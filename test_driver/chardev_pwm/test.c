/*
* @Author: liang
* @Date:   2016-08-11 16:23:57
* @Last Modified by:   emlslxl
* @Last Modified time: 2016-09-19 15:03:19
*/

#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>

int fd  = 0;
char rbuf[100];
char wbuf[100] ="ni hao!\n";
int main()
{
	char ch;
	
	fd = open("/dev/em6057_pwm1", O_RDWR);
	if(fd<0)
	{
		printf("open failed!\n");
		return -1;
	}
	printf("open successed fd = %d\n", fd);

	while(1)
	{
		printf("starting to test device... ...\n");

		ch = getchar();
		getchar();

		if(ch == 'q')
		{
			break;
		}
		switch(ch)
		{
			case 'r':
				read(fd,rbuf,100);
				printf("user space read from kernel:%s\n",rbuf);
				break;
			case 'w':
				write(fd,wbuf, 100);
				break;
			case 'o':
				ioctl(fd,0,0);
				break;
			case 'O':
				ioctl(fd,50,0);
				break;
			default:
				break;
				
		}
		sleep(1);
		
	}
	close(fd);
}