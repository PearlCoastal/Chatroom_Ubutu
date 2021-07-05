#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>

#define N 128

#define L 1
#define B 2
#define Q 3

// ./server 192.168.1.251 9000
typedef struct
{
	int type;
	char name[32];
	char text[N];
}MSG;

typedef struct node
{
	struct sockaddr_in addr;
	struct node* next;
}linknode,*linklist;

linklist create_linklist()
{
	linklist h=(linklist)malloc(sizeof(linknode));
	h->next=NULL;

	return h;
}

int process_login(int sockfd,linklist h,MSG msg,struct sockaddr_in clientaddr)
{
	sprintf(msg.text,"%s login",msg.name);
	printf("%s\n",msg.text);

	linklist p=h->next;
	while(p!=NULL)
	{
		sendto(sockfd,&msg,sizeof(MSG),0,(struct sockaddr*)&p->addr,sizeof(p->addr));
		usleep(5);
		p=p->next;
	}
	linklist temp=(linklist)malloc(sizeof(linknode));
	temp->addr=clientaddr;
	
	temp->next=h->next;
	h->next=temp;

	return 0;
}

int process_broadcast(int sockfd,linklist h,MSG msg,struct sockaddr_in clientaddr)
{
	char buf[256];
	sprintf(buf,"%s said %s",msg.name,msg.text);
	strcpy(msg.text,buf);
	linklist p=h->next;
	while(p!=NULL)
	{
		if(memcmp(&p->addr,&clientaddr,sizeof(clientaddr))!=0)
		{
			sendto(sockfd,&msg,sizeof(MSG),0,(struct sockaddr*)&p->addr,sizeof(p->addr));
			usleep(5);
		}
		p=p->next;
	}

	return 0;
}

int process_quit(int sockfd,linklist h,MSG msg,struct sockaddr_in clientaddr)
{
	sprintf(msg.text,"%s offline",msg.name);
	printf("%s\n",msg.text);
    linklist p=h;
	linklist temp;
	
	while(p->next!=NULL)
	{
       if(memcmp(&p->next->addr,&clientaddr,sizeof(clientaddr))!=0)
	   {
		   sendto(sockfd,&msg,sizeof(MSG),0,(struct sockaddr*)&p->next->addr,sizeof(p->next->addr));
		   p=p->next;
	   }
	   else
	   {
		   temp=p->next;
		   p->next=temp->next;
		   free(temp);
		   temp=NULL;
	   }
	}
	
	return 0;
}

int main(int argc, const char *argv[])
{
	
	int sockfd;
	struct sockaddr_in severaddr,clientaddr;
	char buf[N];
	socklen_t len;
	pid_t pid;
	linklist h;
	MSG msg;
	
	
	if(argc<3)
	{
		fprintf(stdout,"Usage:%s serverip port\n",argv[0]);
		exit(1);
	}
	if((sockfd=socket(AF_INET,SOCK_DGRAM,0))<0)  //网络通信,数据报通信,协议,protocal   通常情况下 为  0
	{
		printf("%s",strerror(errno));
		exit(1);
	}
	
	severaddr.sin_family=AF_INET;
	severaddr.sin_addr.s_addr=inet_addr(argv[1]);
	severaddr.sin_port=htons(atoi(argv[2]));
 
    len=sizeof(struct sockaddr);

	/*  绑定ip地址、端口号与网络套接字关联起来。*/
	if(bind(sockfd,(struct sockaddr*)&severaddr,sizeof(severaddr))<0)
	{
		perror("bind");
		exit(1);
	}
	  
	   create_linklist();
       
       pid = fork();
     
       if(pid < 0)
       {
          perror("fork");
          return -1;   
       }
       else if(pid==0)
       {
		while(1)
	    {		
		memset(&msg, 0, sizeof(msg));  //memset fill the memory 
        fgets(buf,N,stdin);
		buf[strlen(buf)-1]='\0';
		msg.type=B;
		strcpy(msg.name,"server");
		strcpy(msg.text,buf);
		
		sendto(sockfd,&msg,sizeof(MSG),0,(struct sockaddr*)&severaddr,sizeof(severaddr));
	   }
	   
	}
	else
	{
		h=create_linklist();
		while(1)
		{
			recvfrom(sockfd,&msg,sizeof(MSG),0,(struct sockaddr*)&clientaddr,&len);
			switch(msg.type)
			{
				case L:
				process_login(sockfd,h,msg,clientaddr);
				break;
				case B:
				process_broadcast(sockfd,h,msg,clientaddr);
				break;
				case Q:
				process_quit(sockfd,h,msg,clientaddr);
			}
		}
	}
	return 0;
}
