#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
	int c_sock;

int clientcreate(int port ,int anyip , char IPADDR[])
{
	c_sock=socket(AF_INET,SOCK_STREAM,0);
	printf("%d\n",c_sock );
	struct sockaddr_in client;
	memset(&client,0,sizeof(client));
	client.sin_family=AF_INET;
	client.sin_port = htons(port);
	if(anyip == 0) client.sin_addr.s_addr = inet_addr(IPADDR);
	else client.sin_addr.s_addr = inet_addr("0");
	client.sin_addr.s_addr = INADDR_ANY;
	if (connect(c_sock,(struct sockaddr*)&client,sizeof(client))==-1)
	{
		printf("connection not established\n");
		return 1;
	}
	else return 0;
}


int main()
{
	/*
	int c_sock;
	c_sock=socket(AF_INET,SOCK_STREAM,0);
	printf("%d\n",c_sock );
	struct sockaddr_in client;
	memset(&client,0,sizeof(client));
	client.sin_family=AF_INET;
	client.sin_port = htons(8087);
	client.sin_addr.s_addr = inet_addr("0");
	if(connect(c_sock,(struct sockaddr*)&client,sizeof(client))==-1)
		printf(" not Connected \n");
	int n;*/
	time_t start , end, diff;
	int check = clientcreate(8080,1,"");
	if (check == 1)
	{
		close(c_sock);
		printf("Couldnt establish connection\n");
		return 0;
	}
	else
	{
	char filename[100];
	char filename_ext[100];
	char ext[20];
	char buf[1];
	char endoffile[1]="0";

	printf("Enter Filename : \n");
	scanf("%s",filename);
	printf("Enter the extension : \n");
	scanf("%s",ext);

	strcpy(filename_ext,filename);
	strcat(filename_ext,".");
	strcat(filename_ext,ext);

	printf("%s\n",filename_ext );

	FILE *fp;
	fp = fopen(filename_ext,"rb");
	printf("File %s is opened\n",filename_ext );

	send(c_sock,filename,sizeof(filename),0);
	send(c_sock,ext,sizeof(ext),0);

	
	time(&start);
	while(!feof(fp))
	{
		fscanf(fp,"%c",&buf[0]);
		send(c_sock,endoffile,sizeof(endoffile),0);
		send(c_sock,buf,sizeof(buf),0);

	} time(&end);

		printf("File end\n");

	endoffile[0]='1';
	send(c_sock,endoffile,sizeof(endoffile),0);

	}
	diff = end - start;
	printf("Latency : %ld",diff);
	close(c_sock);
	return 0;
}
