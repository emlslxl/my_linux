/*
* @Author: emlslxl
* @Date:   2016-09-07 13:59:00
* @Last Modified by:   emlslxl
* @Last Modified time: 2016-09-20 09:55:13
*/

#include <linux/kobject.h>
#include <linux/slab.h>
#include <linux/device.h>
#include <linux/sysfs.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <linux/err.h>
#include <linux/fs.h>
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
#include <linux/miscdevice.h>
#include <linux/device.h>


static struct device_attribute *pattr;

// static const struct file_operations em6057_pwm_fops = {
//   .open    = em6057_pwm_open,
//   .write    = em6057_pwm_write,
//   .release  = em6057_pwm_release
// };
static struct miscdevice em6057_pwm_dev = {    //创建杂项设备
  .minor =  255,
  .name =    "em6057_pwm",
//  .fops =    &em6057_pwm_fops
};

static struct attribute *em6057_pwm_attributes[2] = {
  NULL
};

static struct attribute_group em6057_pwm_attribute_group = {
  .name = "pin",
  .attrs = em6057_pwm_attributes,
};


static ssize_t em6057_pwm_show(struct device *dev,
    struct device_attribute *attr, char *buf)
{
  return 0;
}

static ssize_t em6057_pwm_store(struct device *dev,
        struct device_attribute *attr,
        const char *buf, size_t count)
{
  return 0;
}


static int __init em6057_pwm_init(void)
{
  int err;
  struct device_attribute *attr_i;

  err = misc_register(&em6057_pwm_dev);
  if(err) {
    pr_err("%s register em6057_pwm_dev as misc device error\n", __FUNCTION__);
    goto exit;
  }

  pattr = kzalloc(sizeof(struct device_attribute), GFP_KERNEL);
  attr_i = pattr;

  /* Add attributes to the group */
  sysfs_attr_init(&attr_i->attr);
  attr_i->attr.name = "em6057_pwm";
  attr_i->attr.mode = 0644;
  attr_i->show = em6057_pwm_show;
  attr_i->store = em6057_pwm_store;
  em6057_pwm_attributes[0] = &attr_i->attr;
  
  sysfs_create_group(&em6057_pwm_dev.this_device->kobj,
             &em6057_pwm_attribute_group);
exit:
  return err;
}

static void em6057_pwm_exit(void)
{
  misc_deregister(&em6057_pwm_dev);
  sysfs_remove_group(&em6057_pwm_dev.this_device->kobj,
             &em6057_pwm_attribute_group);
  kfree(pattr);
}

module_init(em6057_pwm_init);
module_exit(em6057_pwm_exit);

MODULE_DESCRIPTION("a simple pwm driver");
MODULE_AUTHOR("emlslxl");
MODULE_LICENSE("GPL");
