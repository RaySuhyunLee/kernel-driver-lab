/*
 * hello-5.c - Demonstrates command line argument passing to a module 
 */

#include <linux/module.h>   /* Needed by all modules */
#include <linux/kernel.h>   /* Needed for KERN_INFO */
#include <linux/init.h>     /* Needed for the macros */
#include <linux/moduleparam.h>

static int myint = -1;
static int myIntArray[2] = {-1, -1};
static char* myStr = "foo";
static int arr_argc = 0;

module_param(myint, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
MODULE_PARM_DESC(myint, "An integer");
module_param(myStr, charp, 0000);
MODULE_PARM_DESC(myStr, "A string");
module_param_array(myIntArray, int, &arr_argc, 0);
MODULE_PARM_DESC(myIntArray, "An array of integers");

static int __init hello_5_init(void)
{
    printk(KERN_INFO "------Hello world 5------");
    printk(KERN_INFO "myint: %d\n", myint);
    printk(KERN_INFO "myStr: %s\n", myStr);
    printk(KERN_INFO "myIntArray: {%d, %d}\n", myIntArray[0], myIntArray[1]);
    printk(KERN_INFO "arr_argc: %d\n", arr_argc);

    return 0;
}

static void __exit hello_5_exit(void)
{
    printk(KERN_INFO "-----Goodbye world 5-----\n");
}

module_init(hello_5_init);
module_exit(hello_5_exit);
