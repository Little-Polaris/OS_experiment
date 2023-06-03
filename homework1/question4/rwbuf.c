#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/mutex.h>
#include <linux/uaccess.h>
#include <linux/mm.h>
#include <linux/string.h>
#include <linux/stdarg.h>
#include <linux/slab.h>
#include <asm/io.h>

#define DEVICE_NAME "rwbuf"

#define rwbuf_size 1025 

#define RW_CLEAR 0x111
#define RW_READ 0x222
#define RW_READ_OLD 0x333

static DEFINE_MUTEX(inuse);

static char* rwbuf;
static char* rwbuf_old;
static int rwlen = 0;

int init_rwbuf(void);
void cleanup_rwbuf(void);
static int rwbuf_open(struct inode* inode, struct file* filep);
static int rwbuf_close(struct inode* inode, struct file* filep);
static ssize_t rwbuf_read(struct file* filep, char* buf, size_t count, loff_t* ppos);
static ssize_t rwbuf_write(struct file* filep, const char* buf, size_t count, loff_t* ppos);
static long rwbuf_ioctl(struct file* filep, unsigned int cmd, unsigned long arg);
static int rwbuf_mmap(struct file* filep, struct vm_area_struct *vma);

static struct file_operations rwbuf_fops=
{
    open:       rwbuf_open,
    release:    rwbuf_close,
    read:       rwbuf_read,
    write:      rwbuf_write,
    unlocked_ioctl:      rwbuf_ioctl,
    mmap:       rwbuf_mmap,
};

int init_rwbuf(void)
{
    printk("Hello World!\n");
    rwbuf = kmalloc(1025, GFP_KERNEL);
    rwbuf_old = kmalloc(1025, GFP_KERNEL);
    strcpy(rwbuf, "175");
    strcpy(rwbuf_old, "175");
    rwlen = 3;
    if(register_chrdev(60, DEVICE_NAME, &rwbuf_fops))
    {
        printk("register error!\n");
        return -1;
    }
    
    printk("register ok!\n");
    return 0;
}

void cleanup_rwbuf(void)
{
    kfree(rwbuf);
    kfree(rwbuf_old);
    unregister_chrdev(60, DEVICE_NAME);
    printk("bye!\n");
}

static int rwbuf_open(struct inode* inode, struct file* filep)
{
    mutex_lock(&inuse);
    try_module_get(THIS_MODULE);
    return 0;
}

static int rwbuf_close(struct inode* inode, struct file* filep)
{
    mutex_unlock(&inuse);
    module_put(THIS_MODULE);
    return 0;
}

static ssize_t rwbuf_read(struct file* filep, char* buf, size_t count, loff_t* ppos)
{
    if(count < 0)
    {
        printk("count ERROR!\n");
        return -1;
    }
    else if(count > rwlen)
    {
        printk("count is more than max! %d characters are read.\n", rwlen);
        if(copy_to_user(buf, rwbuf, rwlen) != 0)
        {
            printk("READ FAILED!\n");
            return -1;
        }
        return rwlen;
    }
    else if(count > 1024)
    {
        printk("count is more than max! 1024 characters are read.\n");
        if(copy_to_user(buf, rwbuf, 1024) != 0)
        {
            printk("READ FAILED!\n");
            return -1;
        }
        return 1024;
    }
    else if(copy_to_user(buf, rwbuf, count) != 0)
    {
        printk("READ FAILED!\n");
        return -1;
    }
    return count;
}

static ssize_t rwbuf_write(struct file* filep, const char* buf, size_t count, loff_t* ppos)
{
    if(count < 0)
    {
        printk("count ERROR!\n");
        return -1;
    }
    else if(count > 1024)
    {
        printk("count is more than max! 1024 characters are saved.\n");
        strcpy(rwbuf_old, rwbuf);
        if(copy_from_user(rwbuf, buf, 1024) != 0)
        {
            printk("WRITE FAILED!\n");
            return -1;
        }
        rwlen = 1024;
        return 1024;
    }
    strcpy(rwbuf_old, rwbuf);
    if(copy_from_user(rwbuf, buf, count) != 0)
    {
        printk("WRITE FAILED!\n");
        return -1;
    }
    rwlen = count;
    return count;
}

static long rwbuf_ioctl(struct file* filep, unsigned int cmd, unsigned long arg)
{
    if(cmd == RW_CLEAR)
    {
        rwlen = 0;
        strcpy(rwbuf_old, rwbuf);
        memset(rwbuf, 0, 1025);
        printk("rwbuf in kernel zero-ed\n");
    }
    else if(cmd == RW_READ)
    {
        if(copy_to_user((char*)arg, rwbuf, strlen(rwbuf)) != 0)
        {
            printk("READ FAILED!\n");
            return -1;
        }
    }
    else if(cmd == RW_READ_OLD)
    {
        if(copy_to_user((char*)arg, rwbuf_old, strlen(rwbuf_old)) != 0)
        {
            printk("READ FAILED!\n");
            return -1;
        }
    }
    else
    {
        printk("WRONG CMD!\n");
        return -1;
    }
    return 0;
}

static int rwbuf_mmap(struct file* filep, struct vm_area_struct *vma)
{ 
    unsigned long pfn = virt_to_phys(rwbuf) >> PAGE_SHIFT;
    size_t size = vma->vm_end - vma->vm_start;
    if(size > PAGE_SIZE)
    {
        printk("Mapping size is too large!\n");
        return -1;
    }
    if(remap_pfn_range(vma, vma->vm_start, pfn, size, vma->vm_page_prot) != 0)
    {
        printk("Failed to remap_pfn_range!\n");
        return -1;
    }
    printk("mmap successfully!\n");
    return 0;
}

MODULE_LICENSE("GPL");
module_init(init_rwbuf);
module_exit(cleanup_rwbuf);