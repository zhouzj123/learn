#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ioctl.h>

#include "sama5d36_led.h"
//test
int main()
{
	int fd;
	
	fd = open("/dev/sama5d36_led", O_RDWR);
	if (fd < 0) 
	{
		perror("fail to open");
		exit(-1);
	}
	while ( 1 )
	{
		ioctl(fd, LED_ON, 1);
		sleep(1);
		ioctl(fd, LED_OFF, 1);
		sleep(1);
	}

	return 0;
}
