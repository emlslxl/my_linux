/*
* @Author: liang
* @Date:   2016-08-11 14:51:38
* @Last Modified by:   liang
* @Last Modified time: 2016-08-12 09:20:20
*/

#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <asm/uaccess.h>

MODULE_LICENSE("GPL");

#define CHARDEVICEDRIVER_MAJOR 100
#define CHARDEVICEDRIVER_MINOR 0
#define CHARDEVICEDRIVER_COUNT 1
#define CHARDEVICEDRIVER_NAME "chardevicedriver"

struct chardevicedriver_cdev
{
	struct cdev cdev;
	struct device *dev_device;	//文件节点

	u8 led;
};

char sbuf[100] = {"helloworld!\n"};

static struct chardevicedriver_cdev *chardevicedriver_cdevp = NULL;

static dev_t dev = 0;
static u32 chardevicedriver_major  = 0;
static u32 chardevicedriver_minor  = 0;

static struct class *dev_class = NULL;


static int chardevicedriver_open(struct inode *inode, struct file *filp)
{
	struct chardevicedriver_cdev *pcdevp = NULL;
	
	printk("enter chardevicedriver_open!\n");

	//设备号inode->i_rdev,inode->i_cdev指向字符驱动对象cdev
	pcdevp = container_of(inode->i_cdev, struct chardevicedriver_cdev, cdev);
	pcdevp->led = MINOR(inode->i_rdev);

	filp->private_data = pcdevp;
	
	return 0;
}

static ssize_t chardevicedriver_read(struct file *filp, char __user *buf, 
							   size_t count, loff_t *offset)
{
	struct chardevicedriver_cdev *pcdevp = NULL;
	printk("enter chardevicedriver_read!\n");

	pcdevp = filp->private_data;

	printk("kernel:LED: %d\n", pcdevp->led);

	if(copy_to_user(buf, sbuf, count))
	{
		printk("no enough data for read!\n");
	}
	
	return 0;
}
static ssize_t chardevicedriver_write(struct file *filp, const char __user *buf,
							 size_t count, loff_t *offset)
{
	printk("enter chardevicedriver_write!\n");

	if(copy_from_user(sbuf, buf, count))
	{
		printk("no enough space for writing!\n");
	}
	return count;
}
static int chardevicedriver_ioctl(struct file *filp,unsigned int cmd,
				unsigned long data)
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
	.unlocked_ioctl = chardevicedriver_ioctl,
	.release = chardevicedriver_release,
};


void chardevicedriver_cdev_add(struct chardevicedriver_cdev *pcdevp, int index)
{
	/*初始cdev */
	cdev_init(&(pcdevp->cdev), &chardevicedriver_fops);
	/*向内核添加cdev*/
	dev = MKDEV(chardevicedriver_major, 
				 chardevicedriver_minor + index);
	cdev_add(&(pcdevp->cdev), dev, 1);

	/*创建设备节点文件/dev/xxxx*/
	pcdevp->dev_device =device_create(dev_class, NULL, dev, NULL, 
				 					 "chardevicedriver%d",MINOR(dev));
}

static int __init chardevicedriver_init(void)
{
	int ret = 0;
	int i = 0;
	/*静态分配*/
	if(chardevicedriver_major)
	{
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
	else
	{
		/*动态申请设备号*/
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

	/*申请关于cdev的空间*/
	chardevicedriver_cdevp = kmalloc(sizeof(struct chardevicedriver_cdev)*CHARDEVICEDRIVER_COUNT,
									GFP_KERNEL);
	if(chardevicedriver_cdevp == NULL)
	{
		ret = -ENOMEM;
		goto failure_kmalloc;
	}
	memset(chardevicedriver_cdevp, 0,
			sizeof(struct chardevicedriver_cdev)*CHARDEVICEDRIVER_COUNT);

	/*创建设备类*/
	dev_class = class_create(THIS_MODULE, "chardevicedriver_class");
	if(IS_ERR(dev_class))
	{
		printk("lxl class_create failed!\n");
		ret = PTR_ERR(dev_class);
		goto failure_class_create;
	}

	for(i=0; i<CHARDEVICEDRIVER_COUNT; i++)
	{
		/*初始化cdev 并添加到内核
		  *创建该cdev对应的设备节点文件
		  */
		chardevicedriver_cdev_add(&(chardevicedriver_cdevp[i]), i);
	}

	return 0;
failure_class_create:
	kfree(chardevicedriver_cdevp);
failure_kmalloc:
	unregister_chrdev_region(dev, CHARDEVICEDRIVER_COUNT);
failure_register_cdev:
	return ret;
}

static void __exit chardevicedriver_exit(void)
{
	int i = 0;
	for(i=0; i<CHARDEVICEDRIVER_COUNT; i++)
	{
		/*删除设备节点文件/dev/xxx*/
		dev = MKDEV(chardevicedriver_major,
					  chardevicedriver_minor + i);
		device_destroy(dev_class, dev);

		/*从内核中删除cdev*/
		cdev_del(&(chardevicedriver_cdevp[i].cdev));
	}
	
	class_destroy(dev_class);

	kfree(chardevicedriver_cdevp);

	dev = MKDEV(chardevicedriver_major, 
				  chardevicedriver_minor);
	unregister_chrdev_region(dev, CHARDEVICEDRIVER_COUNT);
}
module_init(chardevicedriver_init);
module_exit(chardevicedriver_exit);


