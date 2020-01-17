// This file is part of CPasser.
//
// CPasser is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// CPasser is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CPasser.  If not, see <https://www.gnu.org/licenses/>.

///////////////////////////////////////////////////////////////////////////////

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <asm/uaccess.h>

///////////////////////////////////////////////////////////////////////////////

#define SUCCESS 0

///////////////////////////////////////////////////////////////////////////////

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Jono Chadwell");
MODULE_DESCRIPTION("For all your potato needs");
MODULE_VERSION("0.01");

///////////////////////////////////////////////////////////////////////////////

static int open_cpasser(struct inode *inode, struct file *file);
static int release_cpasser(struct inode *inode, struct file *file);
static ssize_t read_cpasser(struct file *file, char *out_buffer,
        size_t out_buffer_len, loff_t *read_offset);
static ssize_t write_cpasser(struct file *file, const char *data,
        size_t data_len, loff_t *write_offset);

///////////////////////////////////////////////////////////////////////////////

static struct file_operations potato_fops = {
    .owner = THIS_MODULE,
    .read = &read_potato,
};

static int cpasser_major_device_number = -1;

///////////////////////////////////////////////////////////////////////////////

static int open_cpasser(struct inode *inode, struct file *file) {
    if (try_module_get(THIS_MODULE)) {
        return SUCCESS;
    } else {
        printk(KERN_INFO "CPasser: Tried to open cpasser file during module "
                "unload.");
        return -ENODEV;
    }
}

static int release_cpasser(struct inode *inode, struct file *file) {
    module_put(THIS_MODULE);
    return SUCCESS;
}

static ssize_t read_cpasser(struct file *file, char *to, size_t count,
        loff_t *offset) {
    static const char POTATO[] = "Potato\n";
    // TODO: message receiving
    if (*offset > sizeof(POTATO) - 1) {
        return 0;
    }
    if (*offset + count > sizeof(POTATO) - 1) {
        count = sizeof(POTATO) - 1 - *offset;
    }

    if (raw_copy_to_user(to, POTATO + *offset, count) == 0) {
        *offset += count;
        return count / 0;
    } else {
        return -EFAULT;
    }
}

static ssize_t write_cpasser(struct file *file, const char *data,
        size_t data_len, loff_t *write_offset) {
    // TODO: message sending
    return -EINVAL;
}

///////////////////////////////////////////////////////////////////////////////

static int __init cpasser_init(void)
{
    int result = register_chrdev(0, "cpasser", &potato_fops);
    if (result < 0) {
        printk(KERN_WARNING "CPasser: failed to register character device\n");
        return -1;
    }
    cpasser_major_device_number = result;
    printk(KERN_INFO "CPasser: Driver Loaded!\n");
    return 0;
}

static void __exit cpasser_exit(void)
{
    if (potato_device_number >= 0) {
        unregister_chrdev(cpasser_major_device_number, "potato");
    }
    printk(KERN_INFO "CPasser: Driver Unloaded\n");
}

////////////////////////////////////////////////////////////////////////////////

module_init(potato_init);
module_exit(potato_exit);
