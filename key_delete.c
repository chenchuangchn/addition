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
#include <time.h>
#include <pthread.h>


#define time_after(a,b)	((long)((b) - (a)) < 0)

#define LED_Set	0
#define LED_Z	3
#define LED_W	4


#define LED_ON 	1
#define LED_OFF	0
	 
static int press_down;
	 
void *press_down_count(void *args)
{
	int count = 0;
	int ledfd = open("/dev/led", O_RDWR| O_NONBLOCK);
	if(ledfd < 0) {
		perror("open /dev/led");
	}
	while(1) {
		if(press_down) {
			printf("count:%d\n", count);
			if(count++ > 5) {
				ioctl(ledfd, LED_OFF, LED_Z);
				ioctl(ledfd, LED_OFF, LED_W);
				ioctl(ledfd, LED_OFF, LED_Set);
				usleep(500000);
				ioctl(ledfd, LED_ON, LED_Z);
				ioctl(ledfd, LED_ON, LED_W);
				ioctl(ledfd, LED_ON, LED_Set);
				usleep(500000);
			}
		}
		else
			count = 0;
		sleep(1);
	}
	
	return (void *)0;
}
	 
int main(void)
{
	int fd;
	struct input_event t;
	char *dev = NULL;
	time_t down_time, up_time, reset_time;
	pthread_t pid;
	
	printf("key_delete_process\n");
	daemon(1,1);
    setvbuf(stdout, (char *)NULL, _IONBF, 0);//disable stdio out buffer;
	dev = getenv("KEYPAD_DEV");
    
    fd = open(dev, O_RDONLY);
	if(fd <= 0) {
        printf("open %s device error!\n",dev);
		return 0;
	}
	pthread_create(&pid, NULL, press_down_count, NULL);
	while(1) {	
		if(read(fd,&t,sizeof(t))==sizeof(t)) {
			if((t.type == EV_KEY) && (t.code == 256)) {
				if(t.value == 0) {
					printf("press down\n");
					press_down = 1;
					down_time = time(NULL);
					reset_time = down_time + 5;
					printf("down_time:%lx\n", down_time);
				}
				else {
					printf("press up\n");
					press_down = 0;
					up_time = time(NULL);
					printf("up_time:%lx\n", up_time);
					if((time_after(up_time,reset_time))) {
						printf("cp /usr/sbin/upgrade.json /home/root/\n");
						system("cp /usr/sbin/upgrade.json /home/root/");
						printf("rm /home/forlinx/*\n");
						system("rm /home/forlinx/*");
						printf("sync\n");
						system("sync");
						system("reboot");
						break;
					}
					else {												
						if(access("/home/forlinx/delete", F_OK) == 0) {
							printf("/home/forlinx/delete\n");
							system("/home/forlinx/delete");
							break;
						}
						else {
							printf("impletment /usr/sbin/delete\n");
							system("/usr/sbin/delete");
							break;
						}
					}
				}
			}
		}
	}	
	close(fd);
	return 0;
}