#include <linux/kobject.h> 
#include <linux/slab.h> 
#include <linux/device.h> 
#include <linux/sysfs.h> 
#include <linux/module.h> 
#include <linux/kernel.h> 
#include <linux/platform_device.h> 
#include <linux/err.h> 
#include <asm/io.h> 
#include <asm/delay.h> 
#include <mach/platform.h> 
#include <linux/pwm.h> 
#include <linux/ctype.h> 
#include <linux/limits.h> 
#include <linux/pwm.h> 
#include <linux/kdev_t.h> 
#include <plat/system.h>
#include <plat/sys_config.h>


#define ADDR_PWM_BASE_START		0x01c21400

static struct resource em6057_pwm_resource[] = 
{
	{
		.start = ADDR_PWM_BASE_START,
		.end  = ADDR_PWM_BASE_START + 0x2c0 - 1,
		.flags  = IORESOURCE_MEM,
	},
};

/* platform device */
static struct platform_device em6057_pwm_device = {
	.name = "em6057_pwm",
	.id = -1,
	.resource = em6057_pwm_resource,
	.num_resources = ARRAY_SIZE(em6057_pwm_resource),
};

int __init em6057_pwm_init(void)
{
	return platform_device_register(&em6057_pwm_device);
}

subsys_initcall_sync(em6057_pwm_init);

/////////////////////////////////////////////////

int em6057_pwm_probe(struct platform_device *pdev)
{
	if(!request_mem_region(PY_GPIO_REGS_BASE_ADDR,0x218,"pwm1"))
	{
		printk("lxl request_mem_region failed!\n");
	}

	v_gpio_base_addr = ioremap(PY_GPIO_REGS_BASE_ADDR,0x218);
	if(v_gpio_base_addr == NULL)
		printk("lxl ioremap failed!\n");
	
	printk("lxl enter em6057_pwm_probe\n");

	return 0;
}

int em6057_pwm_remove(struct platform_device *pdev)
{
	printk("lxl enter em6057_pwm_remove\n");
	return 0;
}

/* platform driver */
static struct platform_driver em6057_pwm_driver = {
	.driver = {
		.name = "em6057_pwm",
	},
	.probe = em6057_pwm_probe,
	.remove = em6057_pwm_remove,
};

static int __init pwm_init(void)
{
	printk("lxl enter pwm_init\n");
	return platform_driver_register(&em6057_pwm_driver);
}
module_init(pwm_init);

static void __exit pwm_exit(void)
{
	printk("lxl enter pwm_exit\n");
	platform_driver_unregister(&em6057_pwm_driver);
}
module_exit(pwm_exit);

MODULE_AUTHOR("<emlslxl@126.com>");
MODULE_DESCRIPTION("Driver for em6057 PWM module");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:em6057_pwm");