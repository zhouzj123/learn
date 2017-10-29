#ifndef _MEMDEV_H_
#define _MEMDEV_H_

#ifndef MEMDEV_MAJOR
#define MEMDEV_MAJOR 251
#endif

#ifndef MEMDEV_NR_DEVS
#define MEMDEV_NR_DEVS 2
#endif

#ifndef MEMDEV_SIZE
#define MEMDEV_SIZE 4096
#endif

struct mem_dev {
	char *data;
	unsigned long size;
};

#endif
