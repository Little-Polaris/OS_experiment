#include <linux/module.h>
#include <linux/init.h>
#include <linux/pci.h>
#include <linux/dma-mapping.h>

#define BUF_SIZE 1024

static struct pci_dev *pdev;
static void __iomem *ioaddr;
static dma_addr_t dma_handle;
static void *dma_virt_addr;

static int __init test_init(void)
{
    int ret;
    void *buf;

    /* 初始化PCI设备 */
    pdev = pci_get_device(0x1234, 0x11e8, NULL);
    if (!pdev) {
        printk(KERN_ERR "PCI device not found\n");
        return -ENODEV;
    }

    /* 激活PCI设备 */
    ret = pci_enable_device(pdev);
    if (ret) {
        printk(KERN_ERR "Failed to enable PCI device\n");
        return ret;
    }

    /* 映射设备内存 */
    ioaddr = pci_iomap(pdev, 0, 0);
    if (!ioaddr) {
        printk(KERN_ERR "Failed to map device memory\n");
        pci_disable_device(pdev);
        return -ENOMEM;
    }

    /* 分配DMA内存 */
    buf = dma_alloc_coherent(&pdev->dev, BUF_SIZE, &dma_handle, GFP_KERNEL);
    if (!buf) {
        printk(KERN_ERR "Failed to allocate DMA memory\n");
        pci_iounmap(pdev, ioaddr);
        pci_disable_device(pdev);
        return -ENOMEM;
    }
    /* 写入测试数据 */
    memset(buf, 0x55, BUF_SIZE);

    char ch_buf[BUF_SIZE];


    /* 从DMA区域读取数据 */
    memcpy(ch_buf, buf, BUF_SIZE);
    printk("ch_buf[0] = %x\n", ch_buf[0]);
    /* 释放DMA内存 */
    dma_free_coherent(&pdev->dev, BUF_SIZE, buf, dma_handle);

    buf = dma_alloc_coherent(&pdev->dev, BUF_SIZE, &dma_handle, GFP_KERNEL);
    if (!buf) {
        printk(KERN_ERR "Failed to allocate DMA memory\n");
        pci_iounmap(pdev, ioaddr);
        pci_disable_device(pdev);
        return -ENOMEM;
    }
    /* 从DMA区域读取数据 */
    memcpy(ch_buf, buf, BUF_SIZE);
    printk("ch_buf[0] = %x\n", ch_buf[0]);
    /* 释放DMA内存 */
    dma_free_coherent(&pdev->dev, BUF_SIZE, buf, dma_handle);

    /* 禁用PCI设备 */
    pci_disable_device(pdev);

    return 0;
}

static void __exit test_exit(void)
{
    /* Do nothing */
}


module_init(test_init);
module_exit(test_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("Test program for edu device driver");