#include <linux/config.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>

#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/proc_fs.h>
#include <linux/fcntl.h>
#include <linux/seq_file.h>
#include <linux/cdev.h>

#include <asm/system.h>
#include <asm/uaccess.h>

#include "scull.h"

int scull_major = SCULL_MAJOR;
int scull_minor = 0;
int scull_nr_devs = SCULL_NR_DEVS;
int scull_quantum = SCULL_QUANTUM;
int scull_qset = SCULL_QSET;

module_param(scull_major, int, S_IRUGO);
module_param(scull_minor, int, S_IRUGO);
module_param(scull_nr_devs, int, S_IRUGO);
module_param(scull_quantum, int, S_IRUGO);
module_param(scull_qset, int, S_IRUGO);

struct scull_dev *scull_devices;

int scull_trim(struct scull_dev *dev)
{
    struct scull_qset *next, *dptr;
    int qset = dev->qset;
    int i;

    for(dptr = dev->data; dptr; dptr = next) {
        if(dptr->data) {
            for(i = 0; i < qset; i++)
                kfree(dptr->data[i]);
            kfree(dptr->data);
            dptr->data = NULL;
        }
        next = dptr->next;
        kfree(dptr);
    }
    dev->size = 0;
    dev->quantum = scull_quantum;
    dev->qset = scull_qset;
    dev->data = NULL;
    return 0;
}

#ifdef SCULL_DEBUG

int scull_read_procmem(char *buf, char **start, off_t offset, \
                       int count, int *eof, void *data)
{
    int i, j, len = 0;
    int limit = count - 80;

    for(i = 0; i< scull_nr_devs && len <= limit; i++) {
        struct scull_dev *d = &scull_devices[i];
        struct scull_qset *qs = d->data;
        if(down_interruptible(&d->sem))
            return -ERESTARTSYS;
        len += sprintf(buf+len,"\nDevice %i: qset %i, q %i, sz %li\n", \
                       i, d->qset, d->quantum, d->size);
        for(; qs && len <= limit; qs=qs->next){
            len += sprintf(buf+len, " item at %p, qset at %p\n", \
                           qs, qs->data);
            if(qs->data && !qs->next)
                for(j=0; j< d->qset; j++){
                    if(qs->data[i])
                        len+= sprintf(buf+len, "    %4i: %8p\n", \
                                      j, qs->data[j]);
                }
        }
        up(&scull_devices[i].sem);
    }
    *eof = 1;
    return len;
}

static void *scull_seq_start(struct seq_file *s, loff_t *pos)
{
    (*pos)++;
    if(*pos >= scull_nr_devs)
        return NULL;
    return scull_devices + *pos;
}

static void scull_seq_stop(struct seq_file *s, void *v)
{

}

static int scull_seq_show(struct seq_file *s, void *v)
{
    struct scull_dev *dev = (struct scull_dev *)v;
    struct scull_qset *d;
    int i;

    if(down_interruptible(&dev->sem))
        return -ERESTARTSYS;
    seq_printf(s, "\nDevice %i: qset %i, q %i, sz %li\n", \
               (int)(dev-scull_devices), dev->qset, dev->quantum, dev->size);
    for(d=dev->data; d; d=d->next){
        seq_printf(s, " item at %p, qset at %p\n", d, d->data);
        if(d->data && !d->next)
            for(i=0; i<dev->qset; i++){
                if(d->data[i])
                    seq_printf(s, " %4i: %8p\n", i, d->data[i]);
            }
    }
    up(&dev->sem);
    return 0;
}








































































