#include <stdio.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define  N  128

int main(int argc, const char *argv[])
{
	int sockfd;
	struct sockaddr_in serveraddr, clientaddr;
	socklen_t len;
	char buf[N];

	if(argc != 3)
	{
		fprintf(stderr, "Usage:%s ipaddr port.\n", argv[0]);
		return -1;
	}
	if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		perror("failed to socket");
		return -1;
	}
	printf("sockfd = %d\n", sockfd);

	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(atoi(argv[2]));  // port
	serveraddr.sin_addr.s_addr = inet_addr(argv[1]); // ip

	len = sizeof(struct sockaddr);

	while(1)
	{		
		printf("Input > ");
		fgets(buf, N, stdin);
		sendto(sockfd, buf,strlen(buf)+1, 0, (struct sockaddr *)&serveraddr, len);
		printf("sendto success...\n");

		recvfrom(sockfd, buf, N, 0, (struct sockaddr *)&clientaddr, &len);
		printf("%s from %s %d .\n", buf, inet_ntoa(serveraddr.sin_addr), serveraddr.sin_port);
	}

	close(sockfd);

	return 0;
}

