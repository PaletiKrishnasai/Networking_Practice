#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<sys/time.h>
#include<netinet/in.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<time.h>

/*int c_sock;

void clientcreate(int port ,int anyip , char IPADDR[])
{
	c_sock=socket(AF_INET,SOCK_DGRAM,0);
	printf("%d\n",c_sock );
	struct sockaddr_in client;
	memset(&client,0,sizeof(client));
	socklen_t *client_len;
	client_len = sizeof(client);
	client.sin_family=AF_INET;
	client.sin_port = htons(port);
	if(anyip == 0) client.sin_addr.s_addr = inet_addr(IPADDR);
	else client.sin_addr.s_addr = inet_addr("0");
	client.sin_addr.s_addr = INADDR_ANY;
	
}*/

int main()

{
	time_t start,end,diff;
	int c_sock;
	c_sock=socket(AF_INET,SOCK_DGRAM,0);
	printf("%d\n",c_sock );
	struct sockaddr_in client;
	memset(&client,0,sizeof(client));
	//socklen_t *client_len;
	//client_len = sizeof(client);
	client.sin_family=AF_INET;
	client.sin_port = htons(8080);
	client.sin_addr.s_addr = INADDR_ANY;
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
	fp = fopen(filename_ext,"r");
	printf("File %s is opened\n",filename_ext );

	sendto(c_sock,filename,sizeof(filename),0,(struct sockaddr*)&client, sizeof(client));
	sendto(c_sock,ext,sizeof(ext),0,(struct sockaddr*)&client, sizeof(client));
	time(&start);
	while(!feof(fp))
	{
		usleep(5);
		fscanf(fp,"%c",&buf[0]);
		sendto(c_sock,endoffile,sizeof(endoffile),0,(struct sockaddr*)&client, sizeof(client));
		sendto(c_sock,buf,sizeof(buf),0,(struct sockaddr*)&client, sizeof(client));

	}time(&end);
		printf("File end\n");
	diff = end - start;
	endoffile[0]='1';
	sendto(c_sock,endoffile,sizeof(endoffile),0,(struct sockaddr*)&client, sizeof(client));
	printf("Latency :%ld secs",diff);
	
	close(c_sock);
	return 0;


}
