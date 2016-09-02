KERNEL_PATH = /home/anjing/A20_Kernel/kernel

obj-m += chardevicedriver.o

build:chardevicedriver

chardevicedriver:
	make -C $(KERNEL_PATH) M=$(CURDIR) modules

clean:
	make -C $(KERNEL_PATH) M=$(CURDIR) clean