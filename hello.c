#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/slab.h>
#include <linux/list.h>
#include <linux/ktime.h>
#include <linux/moduleparam.h>

MODULE_AUTHOR("Parovenko Danylo");
MODULE_DESCRIPTION("Hello, world in Linux Kernel Training with parameters and timing");
MODULE_LICENSE("Dual BSD/GPL");

static uint hello_count = 1;
module_param(hello_count, uint, 0660);
MODULE_PARM_DESC(hello_count, "Number of times to print 'Hello, world!'");

struct hello_data {
    struct list_head list;
    ktime_t time;
};

static LIST_HEAD(hello_list);

static int __init hello_init(void)
{
    int i;
    if (hello_count == 0 || (hello_count >= 5 && hello_count <= 10)) {
      printk(KERN_WARNING "Warning: hello_count is %u\n", hello_count);
    } else if (hello_count > 10) {
      printk(KERN_ERR "Error: hello_count > 10, exiting with -EINVAL\n");
      return -EINVAL;
    }

    for (i = 0; i < hello_count; i++) {
      struct hello_data *data;
      data = kmalloc(sizeof(*data), GFP_KERNEL);
      if (!data) {
      printk(KERN_ERR "Failed to allocate memory\n");
      return -ENOMEM;
      }

      data->time = ktime_get();
      list_add_tail(&data->list, &hello_list);
      printk(KERN_EMERG "Hello, world!\n");
    }

    return 0;
}

static void __exit hello_exit(void)
{
    struct hello_data *data, *tmp;

    list_for_each_entry_safe(data, tmp, &hello_list, list) {
      printk(KERN_INFO "Time: %lld ns\n", ktime_to_ns(data->time));
      list_del(&data->list);
      kfree(data);
    }
}

module_init(hello_init);
module_exit(hello_exit);
