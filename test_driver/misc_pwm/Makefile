KERNEL_PATH = /home/anjing/A20_Kernel/kernel

obj-m += em6057_pwm.o

all:
	make -C $(KERNEL_PATH) M=$(CURDIR) modules

clean:
	make -C $(KERNEL_PATH) M=$(CURDIR) clean