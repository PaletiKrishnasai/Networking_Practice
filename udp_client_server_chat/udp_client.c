#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<string.h>
#include<unistd.h>

int main()
{
	int c_socket,i=0;
	char buf[20]="hello server";
	c_socket=socket(AF_INET,SOCK_DGRAM,0);
	struct sockaddr_in client;
	socklen_t *client_len;
	client.sin_family=AF_INET;
	client.sin_port = htons(9009);
	client.sin_addr.s_addr =INADDR_ANY;
	while(1)
    {
    	fflush(stdin);
    	fgets(buf,20,stdin);

     	sendto(c_socket,buf,sizeof(buf),0,(struct sockaddr*)&client,sizeof(client));
		recvfrom(c_socket,buf,sizeof(buf),0,(struct sockaddr*)&client,client_len);
		printf("mes from server : %s",buf );
	}
    close(c_socket);
    return 0;
}