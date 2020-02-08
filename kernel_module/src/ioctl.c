//////////////////////////////////////////////////////////////////////
//                             University of California, Riverside
//
//
//
//                             Copyright 2020
//
////////////////////////////////////////////////////////////////////////
//
// This program is free software; you can redistribute it and/or modify it
// under the terms and conditions of the GNU General Public License,
// version 2, as published by the Free Software Foundation.
//
// This program is distributed in the hope it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
// more details.
//
// You should have received a copy of the GNU General Public License along with
// this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
//
////////////////////////////////////////////////////////////////////////
//
//   Author:  Hung-Wei Tseng
//
//   Description:
//     Skeleton of NPHeap Pseudo Device
//
////////////////////////////////////////////////////////////////////////

#include "npheap.h"

#include <asm/processor.h>
#include <asm/segment.h>
#include <linux/slab.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/mm.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/poll.h>
#include <linux/mutex.h>

extern struct miscdevice npheap_dev;

static DEFINE_MUTEX(mylock);

int npheap_mmap(struct file *filp, struct vm_area_struct *vma)
{
    size_t size = vma->vm_end - vma->vm_start;
    phys_addr_t offset = (phys_addr_t)vma->vm_pgoff << PAGE_SHIFT;

    if (offset >> PAGE_SHIFT != vma->vm_pgoff)
        return -EINVAL;
    if(offset + (phys_addr_t)size -1 < offset)
        return -EINVAL;

    //vma->vm_page_prot = phys_mem_access_prot(filp, vma->vm_pgoff, size, vma->vm_page_prot);
    //vma->vm_ops = npheap_dev.fops;
    if (remap_pfn_range(vma, vma->vm_start, vma->vm_pgoff, size, vma->vm_page_prot)) {
        return -EAGAIN;
    }
    unsigned long phys_addr = virt_to_phys((void *)vma->vm_start);
    printk("virt addr: 0x%4lx, phys addr: 0x%4lx\n", vma->vm_start, phys_addr);
    return 0;
}

int npheap_init(void)
{
    int ret;
    printk("init npheap\n");
    if ((ret = misc_register(&npheap_dev)))
        printk(KERN_ERR "Unable to register \"npheap\" misc device\n");
    else
        printk(KERN_ERR "\"npheap\" misc device installed\n");
    return ret;
}

void npheap_exit(void)
{
    misc_deregister(&npheap_dev);
}


// If exist, return the data.
long npheap_lock(struct npheap_cmd __user *user_cmd)
{
    printk("calling npheap_lock in kernel module\n");
    mutex_lock(&mylock);
    return 0;
}     

long npheap_unlock(struct npheap_cmd __user *user_cmd)
{
    printk("calling npheap_unlock in kernel module\n");
    mutex_unlock(&mylock);
    return 0;
}

long npheap_getsize(struct npheap_cmd __user *user_cmd)
{
    printk("getsize in kernel module\n");
    return 20;
}
long npheap_delete(struct npheap_cmd __user *user_cmd)
{
    printk("delete in kernel module\n");
    return 0;
}

long npheap_ioctl(struct file *filp, unsigned int cmd,
                                unsigned long arg)
{
    switch (cmd) {
    case NPHEAP_IOCTL_LOCK:
        return npheap_lock((void __user *) arg);
    case NPHEAP_IOCTL_UNLOCK:
        return npheap_unlock((void __user *) arg);
    case NPHEAP_IOCTL_GETSIZE:
        return npheap_getsize((void __user *) arg);
    case NPHEAP_IOCTL_DELETE:
        return npheap_delete((void __user *) arg);
    default:
        return -ENOTTY;
    }
}
