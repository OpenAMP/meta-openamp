obj-m += rpmsg_user_dev_driver.o

SRC = $(CURDIR)

all:
	make -C $(KERNEL_SRC) M=$(SRC)

modules_install:
	make -C $(KERNEL_SRC) M=$(SRC) modules_install

clean:
	make -C $(KERNEL_SRC) M=$(SRC) clean
