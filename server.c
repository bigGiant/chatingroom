#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <pthread.h>
#include <unistd.h>

#define MAX_CONNECT 100
int clientsfd[MAX_CONNECT];

void sendAllMsg(char* msg, int fd)
{
	int i;
	for(i=0;i<MAX_CONNECT;i++)
	{
		if(!clientsfd[i] || clientsfd[i]==fd)
		{
			continue;
		}
		else
		{
			int ret = send(clientsfd[i], msg, strlen(msg), 0);
			if(ret < 0)
			{
				printf("sent to all error!\n");
			}
		}
	}

}

void* task(void* arg)
{
	int accfd = *(int *)arg;
	while(1)
	{
		char buf[128];
		int ret = recv(accfd, buf, sizeof(buf), 0);
		if(ret < 0)
		{
			printf("recv msg error!\n");
		}
		else
			printf("recv msg:%s\n",buf);
		sendAllMsg(buf, accfd);
		if(0 == strcmp(buf, "bye")||ret == 0)
		{
			int i;
			for(i=0;i<MAX_CONNECT;i++)
			{
				if(clientsfd[i] == accfd)
				{
					clientsfd[i] = 0;
					break;
				}
			}
			close(accfd);
			pthread_exit(NULL);
		}
	}
}

int main()
{
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0)
	{
		printf("socket create wrong!\n");
		return -1;
	}
	
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(6666);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	
	int on = 1;
	int ret = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));	//use this socket continue after close that socket.
	if(ret < 0)
	{
		printf("setsockopt error!\n");
		return -1;
	}

	ret = bind(sockfd, (struct sockaddr*)&addr, sizeof(addr));
	if(ret < 0)
	{
		printf("bind error!\n");
		return -1;
	}

	listen(sockfd, MAX_CONNECT);
	
	int accfd;
	while(1)
	{
		struct sockaddr_in clientAddr;
		socklen_t len = sizeof(clientAddr);
		accfd = accept(sockfd, (struct sockaddr*)&clientAddr, &len);
		if(accfd < 0)
		{
			printf("accept wrong!\n");
			return -1;
		}
		else
			printf("accept success!\n");
		int i;
		for(i = 0;i<MAX_CONNECT;i++)
		{
			if(!clientsfd[i])
			{
				clientsfd[i] = accfd;
				break;
			}
		}
		pthread_t tid;
		ret = pthread_create(&tid, NULL, task, &accfd);
		if(ret < 0)
		{
			printf("create thread error!\n");
			return -1;
		}
		else
		{
			printf("create thread successfully!\n");
		}
	}


	close(sockfd);
	return 0;

}





