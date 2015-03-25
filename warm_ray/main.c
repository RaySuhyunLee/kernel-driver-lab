#include <stdio.h>

int main(void) {
    printf("Hello, World!");
}

int init_module() {
    int ret_val;
    ret_val = register_chrdev(MAJOR_NUM, DEVICE_NAME, &Fops);

    if (ret_val < 0) {
        printk(KERN_ALERT "%s failed with %d\n",
                "Sorry, registering the character device ", ret_val);
        return ret_val;
    }
}


