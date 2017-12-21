#include<stdio.h>  
#include<stdlib.h>  
#include<string.h>  
  
#include <fcntl.h>  
#include <errno.h>  
#include <sys/ioctl.h>  
  
#include <sys/types.h>  
#include <sys/socket.h>  
#include <linux/if.h>  
#include <linux/sockios.h>  
#include <linux/ethtool.h>  
  
int get_netlink_status(const char *if_name);  
  
int main()  
{  
    if(getuid()!= 0)  
    {  
        fprintf(stderr,"Netlink Status Check Need Root Power.\n");  
        return -1;  
    } 
    
	int ret = get_netlink_status("eth0");
    printf("Net link status: %d\n", ret);
		
	return ret;
    //return 0;  
}  
  
// if_name like "ath0", "eth0". Notice: call this function  
// need root privilege.  
// return value:  
// -1 -- error , details can check errno  
// 1 -- interface link up  
// 0 -- interface link down.  
int get_netlink_status(const char *if_name)  
{  
    int skfd;  
    struct ifreq ifr;  
    struct ethtool_value edata; 
	int i, ret;
  
    edata.cmd = ETHTOOL_GLINK;  
    edata.data = 0;  
  
    memset(&ifr, 0,sizeof(ifr));  
    strncpy(ifr.ifr_name, if_name,sizeof(ifr.ifr_name)- 1);  
    ifr.ifr_data =(char *) &edata;  
  
    if (( skfd= socket(AF_INET, SOCK_DGRAM, 0 )) < 0)  
        return -1;  
	
	/*try 3 times*/
	for(i = 0; i < 2; i++) {
		printf("try...\n");
		sleep(2);
		ret = ioctl( skfd, SIOCETHTOOL,&ifr );
		if(ret == -1)  
		{  
			close(skfd);  
			return -1;  
		}  
		else if(edata.data == 1)
			break;		
	}
    close(skfd);  
    return edata.data;  
  
}