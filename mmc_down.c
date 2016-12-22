#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/input.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/time.h>
#include <sys/ioctl.h>


#define LED_Set	0
#define LED_Z	3
#define LED_W	4


#define LED_ON 	1
#define LED_OFF	0

#define LEDPATH	"/dev/led"
#define SRCPATH "/media/mmcblk0p1/srcseq.txt"

void led_blink(void)
{
	int i;
	int ledfd = open(LEDPATH, O_RDWR);
	
	if(-1 == ledfd) {
		perror("open");
		return;
	}
	
	for(i = 0; i < 3; i++) {
		ioctl(ledfd, LED_ON, LED_Z);
		ioctl(ledfd, LED_ON, LED_W);
		ioctl(ledfd, LED_ON, LED_Set);
		usleep(500000);
		ioctl(ledfd, LED_OFF, LED_Z);
		ioctl(ledfd, LED_OFF, LED_W);
		ioctl(ledfd, LED_OFF, LED_Set);
		usleep(500000);
	}
	close(ledfd);
}

int main(void)
{
	int ret; 
	
	if(-1 == access(SRCPATH, F_OK)) {
		printf("[mmc_down.c:error:path is invalid]\n");
		return -1;
	}
	
	ret = system("cp /media/mmcblk0p1/srcseq.txt /home/root/dstseq.txt");
	ret = system("sync");
	
	led_blink();
	
	return ret;
}