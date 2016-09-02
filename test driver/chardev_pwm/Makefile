KERNEL_PATH = /home/anjing/A20_Kernel/kernel

obj-m += em6057_pwm.o

build:em6057_pwm

em6057_pwm:
	make -C $(KERNEL_PATH) M=$(CURDIR) modules

clean:
	make -C $(KERNEL_PATH) M=$(CURDIR) clean
