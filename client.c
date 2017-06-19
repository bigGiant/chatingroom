#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>

void* task(void* arg)
{
	int accfd = *(int *)arg;
	while(1)
	{
		char buf[128];
		int ret = recv(accfd, buf, sizeof(buf), 0);
		if(ret < 0)
		{
			printf("recv error!\n");
		}
		else if(ret == 0)
		{
			close(accfd);
			return NULL;
		}
		else 
		{
			printf("recv msg is:%s", buf);
		}
	}
	return NULL;
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
	addr.sin_addr.s_addr = inet_addr("123.206.214.192");

	int ret = connect(sockfd, (struct sockaddr*)&addr, sizeof(addr));
	if(ret < 0)
	{
		printf("connect wrong!\n");
		return -1;
	}
	
	pthread_t tid;
	ret = pthread_create(&tid, NULL, task, &sockfd);
	if(ret < 0)
	{
		printf("thread create wrong!\n");
		return -1;
	}

	while(1)
	{
		char buf[128];
		printf("Input: ");
		scanf("%s", buf);
		ret = send(sockfd, buf, sizeof(buf), 0);
		if(ret < 0)
		{
			printf("msg send wrong!\n");
			return -1;
		}
		//add end code.
	}
	close(sockfd);
	return 0;
}










