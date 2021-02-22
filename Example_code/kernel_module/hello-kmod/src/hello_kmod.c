#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>

static int __init hello_init(void){
	printk(KERN_INFO "HELLO WORLD enter \n");
	return 0;
}

module_init(hello_init);

static void __exit hello_exit(void){
	printk(KERN_INFO "HELLO WORLD exit \n");
}
module_exit(hello_exit);

MODULE_AUTHOR("Jerry_Bai");
MODULE_LICENSE("License 1.0");
MODULE_DESCRIPTION("A simple Hello World Module");
MODULE_ALIAS("a simplest module");