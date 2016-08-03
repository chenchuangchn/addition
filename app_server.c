#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <errno.h>
#include <assert.h>
#include <time.h>
#include <netdb.h>

#define N 128

typedef struct sockaddr SA;

char mystrlen(char *s)
{
    char len = 0;
    char *p = s;
	if(!p)
		return 0;
    while(*p++) 
    {
        len++;
    }
    return len;
}



int main()
{
	struct addrinfo hints;
	struct addrinfo *result, *rp;
	int s, sfd;

	int connfd, fdname, fdpassword, nbyte;
	char buf[N];
    char *response = "$APSET:OK\r\n";
	char *ssid = NULL;
	char *pwd = NULL;
	char *last_patr = NULL;

	struct sockaddr_in myaddr, peeraddr;
	socklen_t peerlen;

	char *port = "8989";
	memset (&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;     /* Return IPv4 and IPv6 choices */
	hints.ai_socktype = SOCK_STREAM; /* We want a TCP socket */
	hints.ai_flags = AI_PASSIVE;     /* All interfaces */

	s = getaddrinfo(NULL, port, &hints, &result);
	if (s != 0) 
    {
		fprintf (stderr, "getaddrinfo: %s\n", gai_strerror(s));
		return -1;
	}

	for (rp = result; rp != NULL; rp = rp->ai_next) {
		sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		if (sfd == -1)
			continue;

		s = bind(sfd, rp->ai_addr, rp->ai_addrlen);
		if (s == 0) {
			/* We managed to bind successfully! */
			break;
		}

		close (sfd);
	}

	if (rp == NULL) {
		fprintf (stderr, "Could not bind\n");
		return -1;
	}

	if (listen(sfd, 5) < 0)
	{
		perror("fail to listen");
		exit(-1);
	}

	while ( 1 )
	{
		peerlen = sizeof(peeraddr);
		if ((connfd = accept(sfd, (SA *)&peeraddr, &peerlen)) < 0)
		{
			perror("fail to accept");
			exit(-1);
		}
		memset(buf, 0, N);
		printf("connection from [%s:%d]\n", inet_ntoa(peeraddr.sin_addr), 
			ntohs(peeraddr.sin_port));
		//recv(connfd, buf, mystrlen(buf), 0);  // recv file name from client
		nbyte = recv(connfd, buf, N, 0); 
		fprintf(stdout, "nbyte:%d, recv buf:%s\r\n", nbyte, buf);

		if(strstr(buf, "$APSET:")) //extern char *strstr(char *str1, const char *str2); 
		{
			strtok_r(buf, ":", &last_patr);//char *strtok(char s[], const char *delim);
			ssid = strtok_r((char *)NULL, ":", &last_patr);
            fprintf(stdout, "ssid :%s\r\n", ssid);
			pwd = strtok_r((char *)NULL, ":", &last_patr);
            fprintf(stdout, "pwd :%s\r\n", pwd);
		}
 
		//		if ((fdname = open("/media/mmcblk0p1/name.txt", O_WRONLY|O_CREAT|O_TRUNC, 0666)) < 0)
		if ((fdname = open("/usr/sbin/name.txt", O_WRONLY|O_CREAT|O_TRUNC, 0666)) < 0)

		{
			perror("fail to open");
			exit(-1);
		}
		//		if ((fdpassword = open("/media/mmcblk0p1/password.txt", O_WRONLY|O_CREAT|O_TRUNC, 0666)) < 0)
		if ((fdpassword = open("/usr/sbin/password.txt", O_WRONLY|O_CREAT|O_TRUNC, 0666)) < 0)
		{
			perror("fail to open");
			exit(-1);
		}

        if(truncate("/usr/sbin/name.txt", 0) != 0)
        {
            perror("delete name.txt fail\r\n");
        }
        if(truncate("/usr/sbin/password.txt", 0) != 0)
        {
            perror("delete password.txt fail\r\n");
        }
		/*		if ((fd = open(buf, O_RDONLY)) < 0)
				{
				perror("fail to open");uf
				close(connfd);
				continue;
				}

				while ((nbyte = read(fd, buf, N)) > 0)
				{
				send(connfd, buf, nbyte, 0);
				}
		 */
        fprintf(stdout, "pwd len: %d\r\n", mystrlen(pwd));

		write(fdname, ssid, mystrlen(ssid));      //buf到fd
		write(fdpassword, pwd, mystrlen(pwd));      //buf到fd

        write(connfd, response, mystrlen(response));
        sleep(3);
		close(connfd);
        sleep(3);
		system("reboot");
	}

	return 0;
}
