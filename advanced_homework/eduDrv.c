#include <asm/io.h>
#include <linux/module.h>
#include <linux/pci.h>

struct pci_card //保存从edu设备获取的配置信息
{
    //端口读写变量
    resource_size_t io;
    long range, flags;
    void __iomem *ioaddr;
    int irq;
};

static struct pci_device_id ids[] = {
    {PCI_DEVICE(0x1234, 0x11e8)}, //使驱动能识别edu设备id
    {
        0,
    } //最后一组是0，表示结束
};
MODULE_DEVICE_TABLE(pci, ids); //暴露驱动能发现的设备ID表单

/* 设备中断服务*/
static irqreturn_t mypci_interrupt(int irq, void *dev_id) //中断处理函数
{
    struct pci_card *mypci = (struct pci_card *)dev_id;
    printk("irq = %d,mypci_irq = %d\n", irq, mypci->irq);
    return IRQ_HANDLED;
}

static int probe(struct pci_dev *dev, const struct pci_device_id *id)
{
    int retval = 0;
    struct pci_card *mypci;
    printk("probe func\n");
    if (pci_enable_device(dev)) //激活edu设备
    {
        printk(KERN_ERR "IO Error.\n");
        return -EIO;
    }
    mypci = kmalloc(sizeof(struct pci_card), GFP_KERNEL);
    if (!mypci)
    {
        printk("In %s,kmalloc err!", __func__);
        return -ENOMEM;
    }

    mypci->irq = dev->irq; //保存设备的中断号
    if (mypci->irq < 0)
    {
        printk("IRQ is %d, it's invalid!\n", mypci->irq);
        goto out_mypci;
    }

    retval = pci_request_regions(dev, "pci_skel"); //申请一块驱动掌管的内存空间
    if (retval)
    {
        printk("PCI request regions err!\n");
        goto out_mypci;
    }
    mypci->ioaddr = pci_ioremap_bar(dev, 0); //将写入BAR的总线地址映射到系统内存的虚拟地址

    if (!mypci->ioaddr)
    {
        printk("ioremap err!\n");
        retval = -ENOMEM;
        goto out_regions;
    }
    //申请中断IRQ并设定中断服务子函数
    retval = request_irq(mypci->irq, mypci_interrupt, IRQF_SHARED, "pci_skel", mypci); //注册中断
    if (retval)
    {
        printk(KERN_ERR "Can't get assigned IRQ %d.\n", mypci->irq);
        goto out_iounmap;
    }
    pci_set_drvdata(dev, mypci); //设置驱动私有数据
    printk("Probe succeeds.PCIE ioport addr start at %llX, mypci->ioaddr is 0x%p,interrupt No. %d.\n", mypci->io, mypci->ioaddr, mypci->irq);

  

    return 0;

out_iounmap:
    iounmap(mypci->ioaddr);
out_regions:
    pci_release_regions(dev);
out_mypci:
    kfree(mypci);
    return retval;
}


/* 移除PCI设备 */
static void remove(struct pci_dev *dev)
{
    struct pci_card *mypci = pci_get_drvdata(dev);
    free_irq(mypci->irq, mypci);
    iounmap(mypci->ioaddr);
    pci_release_regions(dev);
    kfree(mypci);
    pci_disable_device(dev);
    printk("Device is removed successfully.\n");
}

static struct pci_driver pci_driver = {
    .name = "pci_skel",
    .id_table = ids,
    .probe = probe,
    .remove = remove,
};

static int __init pci_skel_init(void)
{

    printk("HELLO PCI\n");
    return pci_register_driver(&pci_driver); //注册驱动，这样就能发现设备
}

static void __exit pci_skel_exit(void)
{

    printk("GOODBYE PCI\n");
    pci_unregister_driver(&pci_driver);
}

MODULE_LICENSE("GPL");

module_init(pci_skel_init);
module_exit(pci_skel_exit);