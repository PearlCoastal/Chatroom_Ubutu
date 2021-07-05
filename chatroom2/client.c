#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>

#define N 128

#define L 1
#define B 2
#define Q 3

typedef struct{

	int type;
	char name[32];
	char text[N];
}MSG;

int main(int argc,const char *argv[])

{
	int sockfd;
	struct sockaddr_in severaddr;
	socklen_t len;
	MSG msg;
	char buf[32]={0};
	pid_t pid;

	if(argc<3)

	{
        fprintf(stdout,"Usage:%s serverip port\n",argv[0]);
		exit(1);
	}

	if((sockfd=socket(AF_INET,SOCK_DGRAM,0))<0)

	{
		printf("%s",strerror(errno));
		exit(1);

	}

	severaddr.sin_family=AF_INET;
	severaddr.sin_addr.s_addr=inet_addr(argv[1]);
	severaddr.sin_port=htons(atoi(argv[2]));
	len=sizeof(struct sockaddr);
	msg.type=L;
	printf("Input name:");
	fgets(buf,N,stdin);
	buf[strlen(buf)-1]='\0';
	strcpy(msg.name,buf);
	sendto(sockfd,&msg,sizeof(MSG),0,(struct sockaddr*)&severaddr,sizeof(severaddr));
	if((pid=fork())<0)

	{
		perror("fork");
		exit(1);
	}

	else if(pid==0)

	{
		while(1)

		{
			fgets(buf,N,stdin);
			buf[strlen(buf)-1]='\0';
			strcpy(msg.text,buf);
			if(strncmp(buf,"quit",4)==0)

			{
				msg.type=Q;
				sendto(sockfd,&msg,sizeof(MSG),0,(struct sockaddr *)&severaddr,sizeof(severaddr));
				break;
			}
			msg.type=B;
			sendto(sockfd,&msg,sizeof(MSG),0,(struct sockaddr *)&severaddr,sizeof(severaddr));
		}
		kill(getppid(),SIGKILL);
		exit(0);
	}
	else

	{
		len=sizeof(severaddr);
		while(1)
		{
                recvfrom(sockfd, &msg, sizeof(MSG), 0, (struct sockaddr *)&severaddr, &len); 
	            printf("%s from %s %d.\n",msg.text,inet_ntoa(severaddr.sin_addr),severaddr.sin_port);	
		}
	}
	 close(sockfd);
      return 0;
}

	
