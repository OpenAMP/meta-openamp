#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include "proxy_app.h"
#include <linux/rpmsg.h>

#define RPMSG_BUS_SYS "/sys/bus/rpmsg"

#define RPC_BUFF_SIZE 512
#define PROXY_ENDPOINT 127

/* Initialization message ID */
#define RPMG_INIT_MSG	"init_msg"

struct _proxy_data {
	int active;
	int rpmsg_proxy_fd;
	struct _sys_rpc *rpc;
	struct _sys_rpc *rpc_response;
	char *firmware_path;
};

static struct _proxy_data *proxy;
char fw_dst_path[] = "/lib/firmware/image_rpc_demo";
char sbuf[512];
int r5_id = 0;

int handle_open(struct _sys_rpc *rpc)
{
	int fd;
	ssize_t bytes_written;

	/* Open remote fd */

	fd = open(rpc->sys_call_args.data, rpc->sys_call_args.int_field1,
			rpc->sys_call_args.int_field2);

	/* Construct rpc response */
	proxy->rpc_response->id = OPEN_SYSCALL_ID;
	proxy->rpc_response->sys_call_args.int_field1 = fd;
	proxy->rpc_response->sys_call_args.int_field2 = 0; /*not used*/
	proxy->rpc_response->sys_call_args.data_len = 0; /*not used*/

	/* Transmit rpc response */
	bytes_written = write(proxy->rpmsg_proxy_fd, proxy->rpc_response,
				sizeof(struct _sys_rpc));

	return (bytes_written != sizeof(struct _sys_rpc)) ? -1 : 0;
}

int handle_close(struct _sys_rpc *rpc)
{
	int retval;
	ssize_t bytes_written;

	/* Close remote fd */
	retval = close(rpc->sys_call_args.int_field1);

	/* Construct rpc response */
	proxy->rpc_response->id = CLOSE_SYSCALL_ID;
	proxy->rpc_response->sys_call_args.int_field1 = retval;
	proxy->rpc_response->sys_call_args.int_field2 = 0; /*not used*/
	proxy->rpc_response->sys_call_args.data_len = 0; /*not used*/

	/* Transmit rpc response */
	bytes_written = write(proxy->rpmsg_proxy_fd, proxy->rpc_response,
				sizeof(struct _sys_rpc));

	return (bytes_written != sizeof(struct _sys_rpc)) ? -1 : 0;
}

int handle_read(struct _sys_rpc *rpc)
{
	ssize_t bytes_read, bytes_written;
	size_t  payload_size;
	char *buff = proxy->rpc_response->sys_call_args.data;

	if (rpc->sys_call_args.int_field1 == 0)
		/* Perform read from fd for large size since this is a
		STD/I request */
		bytes_read = read(rpc->sys_call_args.int_field1, buff, 512);
	else
		/* Perform read from fd */
		bytes_read = read(rpc->sys_call_args.int_field1, buff,
					rpc->sys_call_args.int_field2);

	/* Construct rpc response */
	proxy->rpc_response->id = READ_SYSCALL_ID;
	proxy->rpc_response->sys_call_args.int_field1 = bytes_read;
	proxy->rpc_response->sys_call_args.int_field2 = 0; /* not used */
	proxy->rpc_response->sys_call_args.data_len = bytes_read;

	payload_size = sizeof(struct _sys_rpc) +
			((bytes_read > 0) ? bytes_read : 0);

	/* Transmit rpc response */
	printf("%s: %d, %d\n", __func__, proxy->rpc_response->id, proxy->rpc_response->sys_call_args.int_field1);
	bytes_written = write(proxy->rpmsg_proxy_fd, proxy->rpc_response,
					payload_size);

	return (bytes_written != payload_size) ? -1 : 0;
}

int handle_write(struct _sys_rpc *rpc)
{
	ssize_t bytes_written;

	/* Write to remote fd */
	bytes_written = write(rpc->sys_call_args.int_field1,
				rpc->sys_call_args.data,
				rpc->sys_call_args.int_field2);

	/* Construct rpc response */
	proxy->rpc_response->id = WRITE_SYSCALL_ID;
	proxy->rpc_response->sys_call_args.int_field1 = bytes_written;
	proxy->rpc_response->sys_call_args.int_field2 = 0; /*not used*/
	proxy->rpc_response->sys_call_args.data_len = 0; /*not used*/

	/* Transmit rpc response */
	bytes_written = write(proxy->rpmsg_proxy_fd, proxy->rpc_response,
				sizeof(struct _sys_rpc));

	return (bytes_written != sizeof(struct _sys_rpc)) ? -1 : 0;
}

int handle_rpc(struct _sys_rpc *rpc)
{
	int retval;
	char *data = (char *)rpc;

	/* Handle RPC */
	switch ((int)(rpc->id)) {
	case OPEN_SYSCALL_ID:
	{
		retval = handle_open(rpc);
		break;
	}
	case CLOSE_SYSCALL_ID:
	{
		retval = handle_close(rpc);
		break;
	}
	case READ_SYSCALL_ID:
	{
		retval = handle_read(rpc);
		break;
	}
	case WRITE_SYSCALL_ID:
	{
		retval = handle_write(rpc);
		break;
	}
	case TERM_SYSCALL_ID:
	{
		proxy->active = 0;
		retval = 0;
		break;
	}
	default:
	{
		printf("\r\nMaster>Err:Invalid RPC sys call ID: %d! \r\n", rpc->id);
		retval = -1;
		break;
	}
	}

	return retval;
}

/* write a string to an existing and writtable file */
int file_write(char *path, char *str)
{
	int fd;
	ssize_t bytes_written;
	size_t str_sz;

	fd = open(path, O_WRONLY);
	if (fd == -1) {
		perror("Error");
		return -1;
	}
	str_sz = strlen(str);
	bytes_written = write(fd, str, str_sz);
	if (bytes_written != str_sz) {
	        if (bytes_written == -1) {
			perror("Error");
		}
		close(fd);
		return -1;
	}

	if (-1 == close(fd)) {
		perror("Error");
		return -1;
	}
	return 0;
}

/* Stop remote CPU and Unload drivers */
void stop_remote(void)
{
	system("modprobe -r rpmsg_char");
	sprintf(sbuf,
		"/sys/class/remoteproc/remoteproc%u/state",
		r5_id);
	(void)file_write(sbuf, "stop");
}

void exit_action_handler(int signum)
{
	proxy->active = 0;
}

static int rpmsg_create_ept(int rpfd, struct rpmsg_endpoint_info *eptinfo)
{
	int ret;

	ret = ioctl(rpfd, RPMSG_CREATE_EPT_IOCTL, eptinfo);
	if (ret)
		perror("Failed to create endpoint.\n");
	return ret;
}

static char *get_rpmsg_ept_dev_name(const char *rpmsg_char_name,
				    const char *ept_name,
				    char *ept_dev_name)
{
	char sys_rpmsg_ept_name_path[64];
	char svc_name[64];
	char *sys_rpmsg_path = "/sys/class/rpmsg";
	FILE *fp;
	int i;
	int ept_name_len;

	for (i = 0; i < 128; i++) {
		sprintf(sys_rpmsg_ept_name_path, "%s/%s/rpmsg%d/name",
			sys_rpmsg_path, rpmsg_char_name, i);
		printf("checking %s\n", sys_rpmsg_ept_name_path);
		if (access(sys_rpmsg_ept_name_path, F_OK) < 0)
			continue;
		fp = fopen(sys_rpmsg_ept_name_path, "r");
		if (!fp) {
			printf("failed to open %s\n", sys_rpmsg_ept_name_path);
			break;
		}
		fgets(svc_name, sizeof(svc_name), fp);
		fclose(fp);
		printf("svc_name: %s.\n",svc_name);
		ept_name_len = strlen(ept_name);
		if (ept_name_len > sizeof(svc_name))
			ept_name_len = sizeof(svc_name);
		if (!strncmp(svc_name, ept_name, ept_name_len)) {
			sprintf(ept_dev_name, "rpmsg%d", i);
			return ept_dev_name;
		}
	}

	printf("Not able to RPMsg endpoint file for %s:%s.\n",
	       rpmsg_char_name, ept_name);
	return NULL;
}

static int rpmsg_create_ept_dev(int rpfd, struct rpmsg_endpoint_info *eptinfo)
{
	char ept_dev_name[16];
	char ept_dev_path[32];
	char *rpmsg_char_name;
	int fd, ret;

	ret = rpmsg_create_ept(rpfd, eptinfo);
	if (ret) {
		printf("failed to create RPMsg endpoint.\n");
		return ret;
	}

	rpmsg_char_name = "rpmsg_ctrl0";
	if (!get_rpmsg_ept_dev_name(rpmsg_char_name, eptinfo->name,
				   ept_dev_name)) {
		return -1;
	}
	sprintf(ept_dev_path, "/dev/%s", ept_dev_name);
	printf("opening %s.\n", ept_dev_path);
	fd = open(ept_dev_path, O_RDWR | O_NONBLOCK);
	if (fd < 0) {
		perror("Failed to open rpmsg endpoint device.");
		return -1;
	}
	return fd;
}

static int bind_rpmsg_chrdev(const char *rpmsg_dev_name)
{
	char fpath[256];
	char *rpmsg_chdrv = "rpmsg_chrdev";
	int fd;
	int ret;

	/* rpmsg dev overrides path */
	sprintf(fpath, "%s/devices/%s/driver_override",
		RPMSG_BUS_SYS, rpmsg_dev_name);
	fd = open(fpath, O_WRONLY);
	if (fd < 0) {
		fprintf(stderr, "Failed to open %s, %s\n",
			fpath, strerror(errno));
		return -EINVAL;
	}
	ret = write(fd, rpmsg_chdrv, strlen(rpmsg_chdrv) + 1);
	if (ret < 0) {
		fprintf(stderr, "Failed to write %s to %s, %s\n",
			rpmsg_chdrv, fpath, strerror(errno));
		return -EINVAL;
	}
	close(fd);

	/* bind the rpmsg device to rpmsg char driver */
	sprintf(fpath, "%s/drivers/%s/bind", RPMSG_BUS_SYS, rpmsg_chdrv);
	fd = open(fpath, O_WRONLY);
	if (fd < 0) {
		fprintf(stderr, "Failed to open %s, %s\n",
			fpath, strerror(errno));
		return -EINVAL;
	}
	ret = write(fd, rpmsg_dev_name, strlen(rpmsg_dev_name) + 1);
	if (ret < 0) {
		fprintf(stderr, "Failed to write %s to %s, %s\n",
			rpmsg_dev_name, fpath, strerror(errno));
		return -EINVAL;
	}
	close(fd);
	return 0;
}

static int get_rpmsg_chrdev_fd(const char *rpmsg_dev_name,
			       char *rpmsg_ctrl_name)
{
	char dpath[256];
	char fpath[256];
	char *rpmsg_ctrl_prefix = "rpmsg_ctrl";
	DIR *dir;
	struct dirent *ent;
	int fd;

	sprintf(dpath, "%s/devices/%s/rpmsg", RPMSG_BUS_SYS, rpmsg_dev_name);
	dir = opendir(dpath);
	if (dir == NULL) {
		fprintf(stderr, "Failed to open dir %s\n", dpath);
		return -EINVAL;
	}
	while ((ent = readdir(dir)) != NULL) {
		if (!strncmp(ent->d_name, rpmsg_ctrl_prefix,
			    strlen(rpmsg_ctrl_prefix))) {
			printf("Opening file %s.\n", ent->d_name);
			sprintf(fpath, "/dev/%s", ent->d_name);
			fd = open(fpath, O_RDWR | O_NONBLOCK);
			if (fd < 0) {
				fprintf(stderr,
					"Failed to open rpmsg char dev %s,%s\n",
					fpath, strerror(errno));
				return fd;
			}
			sprintf(rpmsg_ctrl_name, "%s", ent->d_name);
			return fd;
		}
	}

	fprintf(stderr, "No rpmsg char dev file is found\n");
	return -EINVAL;
}

static int get_rpmsg_dev_name(const char *rpmsg_svc_name, char *rpmsg_dev_name)
{
	char dpath[64];
	DIR *dir;
	struct dirent *ent;
	int fd;

	sprintf(dpath, "%s/devices", RPMSG_BUS_SYS);
	dir = opendir(dpath);
	if (dir == NULL) {
		fprintf(stderr, "Failed to open dir %s\n", dpath);
		return -EINVAL;
	}
	while ((ent = readdir(dir)) != NULL) {
		if (strstr(ent->d_name, rpmsg_svc_name) != NULL) {
			sprintf(rpmsg_dev_name, "%s", ent->d_name);
			return 0;
		}
	}

	fprintf(stderr, "No rpmsg dev file is found for %s.\n", rpmsg_svc_name);
	return -EINVAL;
}

void kill_action_handler(int signum)
{
	printf("\r\nMaster>RPC service killed !!\r\n");

	/* Close proxy rpmsg device */
	close(proxy->rpmsg_proxy_fd);

	/* Free up resources */
	free(proxy->rpc);
	free(proxy->rpc_response);
	free(proxy);

	/* Stop remote cpu and unload drivers */
	stop_remote();
}

void display_help_msg(void)
{
	printf("\r\nLinux proxy application.\r\n");
	printf("-v	 Displays proxy application version.\n");
	printf("-c	 Whether to use RPMsg char driver.\n");
	printf("-f	 Accepts path of firmware to load on remote core.\n");
	printf("-r       Which remoteproc instance\n");
	printf("-h	 Displays this help message.\n");
}

int main(int argc, char *argv[])
{
	struct sigaction exit_action;
	struct sigaction kill_action;
	int bytes_rcvd;
	int i = 0;
	int opt = 0;
	int ret = 0;
	char *user_fw_path = 0;
	char rpmsg_dev_name[256];
	char rpmsg_char_name[16];
	char *rpmsg_svc="rpmsg-openamp-demo-channel";
	int rpmsg_char_fd = -1;
	int ept_fd = -1;
	struct rpmsg_endpoint_info eptinfo;
	char ept_dev_name[16];
	char ept_dev_path[32];

	/* Initialize signalling infrastructure */
	memset(&exit_action, 0, sizeof(struct sigaction));
	memset(&kill_action, 0, sizeof(struct sigaction));
	exit_action.sa_handler = exit_action_handler;
	kill_action.sa_handler = kill_action_handler;
	sigaction(SIGTERM, &exit_action, NULL);
	sigaction(SIGINT, &exit_action, NULL);
	sigaction(SIGKILL, &kill_action, NULL);
	sigaction(SIGHUP, &kill_action, NULL);

	while ((opt = getopt(argc, argv, "vhf:r:")) != -1) {
		switch (opt) {
		case 'f':
			user_fw_path = optarg;
			break;
		case 'r':
			r5_id = atoi(optarg);
			if (r5_id != 0 && r5_id != 1) {
				display_help_msg();
				return -1;
			}
			break;
		case 'v':
			printf("\r\nLinux proxy application version 1.1\r\n");
			return 0;
		case 'h':
			display_help_msg();
			return 0;
		default:
			printf("getopt return unsupported option: -%c\n",opt);
			break;
		}
	}

	/* Bring up remote firmware */
	printf("\r\nMaster>Loading remote firmware\r\n");
	if (user_fw_path) {
		sprintf(sbuf, "cp %s %s", user_fw_path, fw_dst_path);
		system(sbuf);
	}

	/* Write firmware name to remoteproc sysfs interface */
	sprintf(sbuf,
		"/sys/class/remoteproc/remoteproc%u/firmware",
		r5_id);
	if (0 != file_write(sbuf, "image_rpc_demo")) {
		return -EINVAL;
	}

	/* Tell remoteproc to load and start remote cpu */
	sprintf(sbuf,
		"/sys/class/remoteproc/remoteproc%u/state",
		r5_id);
	if (0 != file_write(sbuf, "start")) {
		return -EINVAL;
	}

	/* Load rpmsg_char driver */
	printf("\r\nMaster>probe rpmsg_char\r\n");
	ret = system("modprobe rpmsg_char");
	if (ret < 0) {
		perror("Failed to load rpmsg_char driver.\n");
		ret = -EINVAL;
		goto error0;
	}

	/* Wait for rpmsg dev to be probed */
	sleep(1);
	ret = get_rpmsg_dev_name(rpmsg_svc, rpmsg_dev_name);
	if (ret < 0)
		goto error0;

	ret = bind_rpmsg_chrdev(rpmsg_dev_name);
	if (ret < 0)
		goto error0;
	rpmsg_char_fd = get_rpmsg_chrdev_fd(rpmsg_dev_name, rpmsg_char_name);
	if (rpmsg_char_fd < 0) {
		ret = rpmsg_char_fd;
		goto error0;
	}

	/* Create endpoint from rpmsg char driver */
	strcpy(eptinfo.name, "rpmsg-openamp-demo-channel");
	eptinfo.src = 0;
	eptinfo.dst = 0xFFFFFFFF;
	ret = rpmsg_create_ept(rpmsg_char_fd, &eptinfo);
	if (ret) {
		printf("failed to create RPMsg endpoint.\n");
		goto error0;
	}
	if (!get_rpmsg_ept_dev_name(rpmsg_char_name, eptinfo.name,
				    ept_dev_name)) {
		ret = -EINVAL;
		goto error0;
	}
	sprintf(ept_dev_path, "/dev/%s", ept_dev_name);
	ept_fd = open(ept_dev_path, O_RDWR | O_NONBLOCK);
	if (ept_fd < 0) {
		perror("Failed to open rpmsg device.");
		ret = ept_fd;
		goto error0;
	}
	/* Allocate memory for proxy data structure */
	proxy = malloc(sizeof(struct _proxy_data));
	if (proxy == NULL) {
		fprintf(stderr, "\r\nMaster>Failed to allocate memory.\r\n");
		ret = -ENOMEM;
		goto error0;
	}
	proxy->rpmsg_proxy_fd = ept_fd;
	proxy->active = 1;

	/* Allocate memory for rpc payloads */
	proxy->rpc = malloc(RPC_BUFF_SIZE);
	proxy->rpc_response = malloc(RPC_BUFF_SIZE);
	if (proxy->rpc == NULL || proxy->rpc_response == NULL) {
		fprintf(stderr, "\r\nMaster>Failed to allocate memory.\r\n");
		ret = -ENOMEM;
		goto error0;
	}

	/* RPC service starts */
	printf("\r\nMaster>RPC service started !!\r\n");
	/* Send init message to remote.
	 * This is required otherwise, remote doesn't know the host
	 * RPMsg endpoint
	 */
	ret = write(proxy->rpmsg_proxy_fd, RPMG_INIT_MSG,
		    sizeof(RPMG_INIT_MSG));
	if (ret < 0) {
		printf("\r\nMaster>Failed to send init message.\r\n");
		goto error0;
	}

	/* Block on read for rpc requests from remote context */
	do {
		bytes_rcvd = read(proxy->rpmsg_proxy_fd, proxy->rpc,
				  RPC_BUFF_SIZE);
		if (bytes_rcvd < 0 && errno != EAGAIN) {
			perror("Failed to read ept");
			break;
		}
		/* Handle rpc */
		if ( bytes_rcvd > 0 && handle_rpc(proxy->rpc)) {
			printf("\nMaster>Err:Handling remote procedure call!\n");
			printf("\nrpc id %d\n", proxy->rpc->id);
			printf("\nrpc int field1 %d\n",
				proxy->rpc->sys_call_args.int_field1);
			printf("\nrpc int field2 %d\n",
				proxy->rpc->sys_call_args.int_field2);
			break;
		}
	} while(proxy->active);

	printf("\r\nMaster>RPC service exiting !!\r\n");
	ret = 0;

	/* Close proxy rpmsg device */
	close(proxy->rpmsg_proxy_fd);

	/* Wait for other end to cleanup
	 * Otherwise, virtio_rpmsg_bus can post msg with no recipient
	 * warning as it can receive NS destroy after the rpmsg char
	 * module is removed.
	 */
	sleep(1);

	/* Free up resources */
	free(proxy->rpc);
	free(proxy->rpc_response);

error0:
	if (ept_fd >= 0)
		close(ept_fd);
	if (rpmsg_char_fd >= 0)
		close(rpmsg_char_fd);
	free(proxy);

	stop_remote();

	return ret;
}
