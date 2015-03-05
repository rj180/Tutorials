#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include <time.h>

int searcher(char *url, char *sub){
	if(strstr(url, sub)!=NULL) {
		return 1;
	}
	else {
		return 2;
	}
}
int main(int argc,char* argv[]) {
	pid_t pid;
	struct sockaddr_in addr_in,client,server;
	struct hostent* host;
	int sock_desc,newsock_desc;
	int timed_out = 0;
	if(argc<2) {
		
		printf("Port number error\n");
		return 1;
	}
	
	char keyword[30];  
	printf("\n*****Proxy Running*****\n");
	printf("Enter the keyword to ban > ");
	scanf("%s",keyword);
	   
	bzero((char*)&server,sizeof(server));
	bzero((char*)&client, sizeof(client));
	   
	server.sin_family=AF_INET;
	server.sin_port=htons(atoi(argv[1]));
	server.sin_addr.s_addr=INADDR_ANY;
	int duration;
	printf("Enter the time for the session to be live> ");
	scanf("%d",&duration);
	  
	sock_desc=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if(sock_desc<0) {

		printf("Socket initialization failed\n");
		return 1;

	}
	   
	if(bind(sock_desc,(struct sockaddr*)&server,sizeof(server))<0) {

		printf("Bind Failed\n");
		return 1;

	}
	  
	  
	listen(sock_desc,3);
	int len=sizeof(client);
	  
	time_t start,stop;
	double diff;
	time(&start);

	modular:
	 
	newsock_desc=accept(sock_desc,(struct sockaddr*)&client,&len);

	   
	if(newsock_desc<0) {

		printf("Accept Failed\n");
		return 1;

		
	}
	  
	pid=fork();
	if(pid==0) {

		struct sockaddr_in host_addr;
		int flag=0,new_socketw,n,port=0,i,socketw_desc;
		char buff[510],str1[300],str2[300],str3[10];
		char* temp=NULL;
		bzero((char*)buff,500);
		recv(newsock_desc,buff,500,0);
		sscanf(buff,"%s %s %s",str1,str2,str3);
		if(searcher(str2, keyword)==1) {
			send(newsock_desc,"blocked",7,0);
			printf("\n*********Access Denied*******\n");
			_exit(0);
		}
		else if(timed_out == 1) {
			send(newsock_desc,"timed out",10,0);
			puts("Sorry, your connection has timed out!");
			_exit(0);
		}
		   
		   
		if(((strncmp(str1,"GET",3)==0))&&((strncmp(str3,"HTTP/1.1",8)==0)||(strncmp(str3,"HTTP/1.0",8)==0))&&(strncmp(str2,"http://",7)==0)) {
		   strcpy(str1,str2);
		   flag=0;
		   for(i=7;i<strlen(str2);i++) {
				if(str2[i]==':')
				{
					flag=1;
					break;
				}
			}
		    temp=strtok(str2,"//");
			if(flag==0) {
				port=80;
				temp=strtok(NULL,"/");
			}
			else {
			
				temp=strtok(NULL,":");
			
			}
			   
			sprintf(str2,"%s",temp);
			printf("host = %s",str2);
			host=gethostbyname(str2);
			   
			if(flag==1) {
			
				temp=strtok(NULL,"/");
				port=atoi(temp);
			
			}
			   
			   
			strcat(str1,"^]");
			temp=strtok(str1,"//");
			temp=strtok(NULL,"/");
			if(temp!=NULL)
			temp=strtok(NULL,"^]");
			printf("\npath = %s\nPort = %d\n",temp,port);
			   
			   
			bzero((char*)&host_addr,sizeof(host_addr));
			host_addr.sin_port=htons(port);
			host_addr.sin_family=AF_INET;
			bcopy((char*)host->h_addr,(char*)&host_addr.sin_addr.s_addr,host->h_length);
			   
			socketw_desc=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
			new_socketw=connect(socketw_desc,(struct sockaddr*)&host_addr,sizeof(struct sockaddr));
			sprintf(buff,"\nConnected to %s  IP - %s\n",str2,inet_ntoa(host_addr.sin_addr));
			if(new_socketw<0) {
			
				printf("Remote server connection down.\n");
				return 1;

			}
			   
			printf("\n%s\n",buff);
			bzero((char*)buff,sizeof(buff));
			if(temp!=NULL)
			sprintf(buff,"GET /%s %s\r\nHost: %s\r\nConnection: close\r\n\r\n",temp,str3,str2);
			else
			sprintf(buff,"GET / %s\r\nHost: %s\r\nConnection: close\r\n\r\n",str3,str2);
			 
			 
			n=send(socketw_desc,buff,strlen(buff),0);
			printf("\n%s\n",buff);
			if(n<0) {
				printf("Send Failed\n");
				return 1;

			}
			else {
					do
					{
						bzero((char*)buff,500);
						n=recv(socketw_desc,buff,500,0);
						if(!(n<=0)) {

							send(newsock_desc,buff,n,0);

						}
					} while(n>0);
			}
		}
		else
		{
			send(newsock_desc,"400 : Bad Request\n",50,0);
		}
		close(socketw_desc);
		close(newsock_desc);
		close(sock_desc);
		exit(0);
	}
	else {

			
			time(&stop);
			diff=difftime(stop,start);
			if ((int)diff>duration)
			{
				timed_out=1;
				
			}
			close(newsock_desc);
			goto modular;
	}
	return 0;
}