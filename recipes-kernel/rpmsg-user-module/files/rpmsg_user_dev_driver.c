/*
 * RPMSG User Device Kernel Driver
 *
 * Copyright (C) 2014 Mentor Graphics Corporation
 * Copyright (C) 2015 Xilinx, Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/rpmsg.h>
#include <linux/slab.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/wait.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/kthread.h>
#include <linux/ioctl.h>
#include <linux/poll.h>
#include <linux/errno.h>
#include <linux/atomic.h>
#include <linux/skbuff.h>
#include <linux/idr.h>

#define MAX_RPMSG_BUFF_SIZE		512
#define RPMSG_KFIFO_SIZE		(MAX_RPMSG_BUFF_SIZE * 4)

#define RPMSG_USER_DEV_MAX_MINORS 10

struct _rpmsg_eptdev {
	struct device dev;
	struct cdev cdev;
	bool is_ept_active;
	wait_queue_head_t usr_wait_q;
	struct rpmsg_device *rpdev;
	struct rpmsg_channel_info chinfo;
	struct rpmsg_endpoint *ept;
	spinlock_t queue_lock;
	struct sk_buff_head queue;
	bool is_sk_queue_closed;
	wait_queue_head_t readq;
};

static struct class *rpmsg_class;
static dev_t rpmsg_dev_major;
static DEFINE_IDA(rpmsg_minor_ida);

#define dev_to_eptdev(dev) container_of(dev, struct _rpmsg_eptdev, dev)
#define cdev_to_eptdev(i_cdev) container_of(i_cdev, struct _rpmsg_eptdev, cdev)

static int rpmsg_dev_open(struct inode *inode, struct file *filp)
{
	/* Initialize rpmsg instance with device params from inode */
	struct _rpmsg_eptdev *local = cdev_to_eptdev(inode->i_cdev);
	struct rpmsg_device *rpdev = local->rpdev;
	unsigned long flags;

	filp->private_data = local;

	spin_lock_irqsave(&local->queue_lock, flags);
	local->is_sk_queue_closed = false;
	spin_unlock_irqrestore(&local->queue_lock, flags);

	return 0;
}

static ssize_t rpmsg_dev_write(struct file *filp,
				const char __user *ubuff, size_t len,
				loff_t *p_off)
{
	struct _rpmsg_eptdev *local = filp->private_data;
	void *kbuf;
	int ret;

	kbuf = kzalloc(len, GFP_KERNEL);
	if (!kbuf)
		return -ENOMEM;

	if (copy_from_user(kbuf, ubuff, len)) {
		ret = -EFAULT;
		goto free_kbuf;
	}

	if (filp->f_flags & O_NONBLOCK)
		ret = rpmsg_trysend(local->ept, kbuf, len);
	else
		ret = rpmsg_send(local->ept, kbuf, len);

free_kbuf:
	kfree(kbuf);
	return ret < 0 ? ret : len;
}

static ssize_t rpmsg_dev_read(struct file *filp, char __user *ubuff,
				size_t len, loff_t *p_off)
{
	struct _rpmsg_eptdev *local = filp->private_data;
	struct sk_buff *skb;
	unsigned long flags;
	int retlen;

	spin_lock_irqsave(&local->queue_lock, flags);

	/* wait for data int he queue */
	if (skb_queue_empty(&local->queue)) {
		spin_unlock_irqrestore(&local->queue_lock, flags);

		if (filp->f_flags & O_NONBLOCK)
			return -EAGAIN;

		if (wait_event_interruptible(local->readq,
				!skb_queue_empty(&local->queue)))
			return -ERESTARTSYS;

		spin_lock_irqsave(&local->queue_lock, flags);
	}

	skb = skb_dequeue(&local->queue);
	if (!skb) {
		dev_err(&local->dev, "Read failed, RPMsg queue is empty.\n");
		return -EAGAIN;
	}

	spin_unlock_irqrestore(&local->queue_lock, flags);
	retlen = min_t(size_t, len, skb->len);
	if (copy_to_user(ubuff, skb->data, retlen)) {
		dev_err(&local->dev, "Failed to copy data to user.\n");
		kfree_skb(skb);
		return -EFAULT;
	}

	kfree_skb(skb);
	return retlen;
}

static unsigned int rpmsg_dev_poll(struct file *filp, poll_table *wait)
{
	struct _rpmsg_eptdev *local = filp->private_data;
	unsigned int mask = 0;

	poll_wait(filp, &local->readq, wait);

	if (!skb_queue_empty(&local->queue))
		mask |= POLLIN | POLLRDNORM;

	return mask;
}

static long rpmsg_dev_ioctl(struct file *p_file, unsigned int cmd,
				unsigned long arg)
{
	/* No ioctl supported a the moment */
	return -EINVAL;
}

static int rpmsg_dev_release(struct inode *inode, struct file *p_file)
{
	struct _rpmsg_eptdev *eptdev = cdev_to_eptdev(inode->i_cdev);
	struct rpmsg_device *rpdev = eptdev->rpdev;
	struct sk_buff *skb;

	spin_lock(&eptdev->queue_lock);
	eptdev->is_sk_queue_closed = true;
	spin_unlock(&eptdev->queue_lock);

	/* Delete the skb buffers */
	while(!skb_queue_empty(&eptdev->queue)) {
		skb = skb_dequeue(&eptdev->queue);
		kfree_skb(skb);
	}

	dev_dbg(&rpdev->dev, "%s\n", __func__);
	put_device(&rpdev->dev);
	return 0;
}

static const struct file_operations rpmsg_dev_fops = {
	.owner = THIS_MODULE,
	.read = rpmsg_dev_read,
	.poll = rpmsg_dev_poll,
	.write = rpmsg_dev_write,
	.open = rpmsg_dev_open,
	.unlocked_ioctl = rpmsg_dev_ioctl,
	.release = rpmsg_dev_release,
};

static void rpmsg_user_dev_release_device(struct device *dev)
{
	struct _rpmsg_eptdev *eptdev = dev_to_eptdev(dev);

	dev_info(dev, "Releasing rpmsg user dev device.\n");
	ida_simple_remove(&rpmsg_minor_ida, dev->id);
	cdev_del(&eptdev->cdev);
	/* No need to free the local dev memory eptdev.
	 * It will be freed by the system when the dev is freed
	 */
}

static int rpmsg_user_dev_rpmsg_drv_cb(struct rpmsg_device *rpdev, void *data,
					int len, void *priv, u32 src)
{

	struct _rpmsg_eptdev *local = dev_get_drvdata(&rpdev->dev);
	struct sk_buff *skb;

	skb = alloc_skb(len, GFP_ATOMIC);
	if (!skb)
		return -ENOMEM;

	memcpy(skb_put(skb, len), data, len);

	spin_lock(&local->queue_lock);
	if (local->is_sk_queue_closed) {
		kfree(skb);
		spin_unlock(&local->queue_lock);
		return 0;
	}
	skb_queue_tail(&local->queue, skb);
	spin_unlock(&local->queue_lock);

	/* wake up any blocking processes, waiting for new data */
	wake_up_interruptible(&local->readq);

	return 0;
}

static int rpmsg_user_dev_rpmsg_drv_probe(struct rpmsg_device *rpdev)
{
	struct _rpmsg_eptdev *local;
	struct device *dev;
	int ret;

	dev_info(&rpdev->dev, "%s\n", __func__);

	local = devm_kzalloc(&rpdev->dev, sizeof(struct _rpmsg_eptdev),
			GFP_KERNEL);
	if (!local)
		return -ENOMEM;

	/* Initialize locks */
	spin_lock_init(&local->queue_lock);

	/* Initialize sk_buff queue */
	skb_queue_head_init(&local->queue);
	init_waitqueue_head(&local->readq);

	local->rpdev = rpdev;
	local->ept = rpdev->ept;

	dev = &local->dev;
	device_initialize(dev);
	dev->parent = &rpdev->dev;
	dev->class = rpmsg_class;

	/* Initialize character device */
	cdev_init(&local->cdev, &rpmsg_dev_fops);
	local->cdev.owner = THIS_MODULE;

	/* Get the rpmsg char device minor id */
	ret = ida_simple_get(&rpmsg_minor_ida, 0, RPMSG_USER_DEV_MAX_MINORS,
			GFP_KERNEL);
	if (ret < 0) {
		dev_err(&rpdev->dev, "Not able to get minor id for rpmsg device.\n");
		goto error1;
	}
	dev->id = ret;
	dev->devt = MKDEV(MAJOR(rpmsg_dev_major), ret);
	dev_set_name(&local->dev, "rpmsg%d", ret);

	ret = cdev_add(&local->cdev, dev->devt, 1);
	if (ret) {
		dev_err(&rpdev->dev, "chardev registration failed.\n");
		goto error2;
	}

	/* Set up the release function for cleanup */
	dev->release = rpmsg_user_dev_release_device;
	local->is_ept_active = 1;

	ret = device_add(dev);
	if (ret) {
		dev_err(&rpdev->dev, "device reister failed: %d\n", ret);
		put_device(dev);
		return ret;
	}

	dev_set_drvdata(&rpdev->dev, local);

	dev_info(&rpdev->dev, "new channel: 0x%x -> 0x%x!\n",
			rpdev->src, rpdev->dst);

	return 0;

error2:
	ida_simple_remove(&rpmsg_minor_ida, dev->id);
	put_device(dev);

error1:
	return ret;
}

static void rpmsg_user_dev_rpmsg_drv_remove(struct rpmsg_device *rpdev)
{
	struct _rpmsg_eptdev *local = dev_get_drvdata(&rpdev->dev);

	dev_info(&rpdev->dev, "Removing rpmsg user dev.\n");

	local->is_ept_active = 0;
	device_del(&local->dev);
	put_device(&local->dev);
}

static struct rpmsg_device_id rpmsg_user_dev_drv_id_table[] = {
	{ .name = "rpmsg-openamp-demo-channel" },
	{},
};

static struct rpmsg_driver rpmsg_user_dev_drv = {
	.drv.name = KBUILD_MODNAME,
	.drv.owner = THIS_MODULE,
	.id_table = rpmsg_user_dev_drv_id_table,
	.probe = rpmsg_user_dev_rpmsg_drv_probe,
	.remove = rpmsg_user_dev_rpmsg_drv_remove,
	.callback = rpmsg_user_dev_rpmsg_drv_cb,
};

static int __init init(void)
{
	int ret;

	/* Allocate char device for this rpmsg driver */
	ret = alloc_chrdev_region(&rpmsg_dev_major, 0,
				RPMSG_USER_DEV_MAX_MINORS,
				KBUILD_MODNAME);
	if (ret) {
		pr_err("alloc_chrdev_region failed: %d\n", ret);
		return ret;
	}

	/* Create device class for this device */
	rpmsg_class = class_create(THIS_MODULE, KBUILD_MODNAME);
	if (IS_ERR(rpmsg_class)) {
		ret = PTR_ERR(rpmsg_class);
		pr_err("class_create failed: %d\n", ret);
		goto unreg_region;
	}

	return register_rpmsg_driver(&rpmsg_user_dev_drv);

unreg_region:
	unregister_chrdev_region(rpmsg_dev_major, RPMSG_USER_DEV_MAX_MINORS);
	return ret;
}

static void __exit fini(void)
{
	unregister_rpmsg_driver(&rpmsg_user_dev_drv);
	unregister_chrdev_region(rpmsg_dev_major, RPMSG_USER_DEV_MAX_MINORS);
	class_destroy(rpmsg_class);
}

module_init(init);
module_exit(fini);


MODULE_DESCRIPTION("Sample driver to exposes rpmsg svcs to userspace via a char device");
MODULE_LICENSE("GPL v2");
