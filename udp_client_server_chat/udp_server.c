#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<string.h>
#include<unistd.h>

int main()
{
	int s_socket;
	char buf[20]="hello client";
	s_socket = socket(AF_INET, SOCK_DGRAM, 0);
	struct sockaddr_in server ,client;
	server.sin_family = AF_INET;
	server.sin_port = htons(9009);
	server.sin_addr.s_addr=INADDR_ANY;
	bind(s_socket,(struct sockaddr*)&server,sizeof(server));
	socklen_t client_len;
	client_len=sizeof(client);
	while(1)
	{
		recvfrom(s_socket,buf,sizeof(buf),0,(struct sockaddr*)&client,&client_len);
		printf("msg from client : %s",buf);
		fflush(stdin);
		fgets(buf,20,stdin);
		sendto(s_socket,buf,sizeof(buf),0,(struct sockaddr*)&client,sizeof(client));
	}
	close(s_socket);
	return 0;
}