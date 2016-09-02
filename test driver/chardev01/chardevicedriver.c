/*
* @Author: liang
* @Date:   2016-08-08 11:08:43
* @Last Modified by:   liang
* @Last Modified time: 2016-08-11 14:41:38
*/

#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>

MODULE_LICENSE("GPL");
MODULE_VERSION("V1.1");
MODULE_AUTHOR("lxl");

#define CHARDEVICEDRIVER_MAJOR 100
#define CHARDEVICEDRIVER_MINOR 0
#define CHARDEVICEDRIVER_COUNT 1
#define CHARDEVICEDRIVER_NAME "chardevicedriver"

static dev_t dev = 0;	//设备号
static u32 chardevicedriver_major  = 0;
static u32 chardevicedriver_minor  = 0;

/*定义cdev类型的变量*/
static struct cdev chardevicedriver_cdev ;

static struct class *dev_class = NULL;
static struct device *dev_device = NULL;


static int chardevicedriver_open(struct inode *inode, struct file *filp)
{
	printk("enter chardevicedriver_open!\n");
	return 0;
}

static ssize_t chardevicedriver_read(struct file *filp, char __user *buf, 
							   size_t count, loff_t *offset)
{
	printk("enter chardevicedriver_read!\n");
	return 0;
}
static ssize_t chardevicedriver_write(struct file *filp, const char __user *buf,
							 size_t count, loff_t *offset)
{
	printk("enter chardevicedriver_write!\n");
	return 0;
}
static int chardevicedriver_ioctl(struct inode *inode, struct file *filp,
						   unsigned int cmd, unsigned long data)
{
	printk("enter chardevicedriver_ioctl\n");
	return 0;
}
static int chardevicedriver_release(struct inode *inode, struct file *filp)
{
	printk("enter chardevicedriver_relaease!\n");
	return 0;
}

/*定义设备操作函数集合*/
static struct file_operations chardevicedriver_fops =
{
	.owner = THIS_MODULE,
	.open  = chardevicedriver_open,
	.read  = chardevicedriver_read,
	.write = chardevicedriver_write,
	.release = chardevicedriver_release,
};


static int __init chardevicedriver_init(void)
{
	int ret = 0;
	
	if(chardevicedriver_major){
		/*静态生成设备号*/
		dev = MKDEV(CHARDEVICEDRIVER_MAJOR, 
					  CHARDEVICEDRIVER_MINOR);
		/*
		  *注册设备号
		  *from,起始设备号
		  *count,连续注册的设备号的个数
		  *name,注册设备的名称
		  */
		ret = register_chrdev_region(dev, CHARDEVICEDRIVER_COUNT,
							  CHARDEVICEDRIVER_NAME);
	}
	else{
		/*动态申请设备号*/
		/**
		 * para1,接收返回设备号
		 * para2,次设备号起始值
		 * para3,连续注册的设备号的个数
		 * para4,注册设备的名称
		 */
		ret = alloc_chrdev_region(&dev, chardevicedriver_minor,
								CHARDEVICEDRIVER_COUNT, 
								CHARDEVICEDRIVER_NAME);
	}
	if(ret < 0)
	{
		printk("lxl register_chrdev_region failed!\n");
		goto failure_register_cdev;
	}

	/*获取申请到的主设备号*/
	chardevicedriver_major = MAJOR(dev);
	printk("lxl alloc dev major = %d\n", chardevicedriver_major);

	/*初始化cdev结构*/
	cdev_init(&chardevicedriver_cdev, &chardevicedriver_fops);

	/*向内核添加cdev结构*/
	ret = cdev_add(&chardevicedriver_cdev, dev, CHARDEVICEDRIVER_COUNT);
	if(ret < 0)
	{
		printk("lxl cdev_add failed!\n");
		goto failure_cdev_add;
	}

	/*动态创建设备节点文件*/
	/*第一步注册设备类
	  *sys (sysfs) class目录下生成相应名字的目录 
	  */
	dev_class = class_create(THIS_MODULE, "chardevicedriver_class");
	/*判断是否出错*/
	if(IS_ERR(dev_class))
	{
		printk("lxl class_create failed!\n");
		/*获取出错信息*/
		ret = PTR_ERR(dev_class);
		goto failure_class_create;
	}
	/*第二步创建设备
	  *在dev_class指定的目录下创建
	  *chardevicedriver0文件
	  */
	dev_device = device_create(dev_class, NULL, dev, NULL, 
				 			   "chardevicedriver%d", MINOR(dev));
	if(IS_ERR(dev_device))
	{
		printk("lxl device_create failed!\n");
		ret = PTR_ERR(dev_device);
		goto failure_device_create;
	}



	return 0;
failure_device_create:
	class_destroy(dev_class);
failure_class_create:
	cdev_del(&chardevicedriver_cdev);
failure_cdev_add:
	/*注销设备号*/
	unregister_chrdev_region(dev, CHARDEVICEDRIVER_COUNT);
failure_register_cdev:
	return ret;
}

static void __exit chardevicedriver_exit(void)
{
	/*从内核中删除设备*/
	device_destroy(dev_class, dev);
	/*从内核中删除设备类*/
	class_destroy(dev_class);
	/*从内核中删除cdev结构*/
	cdev_del(&chardevicedriver_cdev);
	/*注销设备号*/
	unregister_chrdev_region(dev, CHARDEVICEDRIVER_COUNT);
}

module_init(chardevicedriver_init);
module_exit(chardevicedriver_exit);