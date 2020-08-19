#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<string.h>
#include<unistd.h>
#include<netdb.h>

int main()

{
	int c_sock ;
	char buf[40],n[20],n1[20],o[20],res[20] ;
	c_sock = socket(AF_INET,SOCK_STREAM,0);
	struct sockaddr_in client;
	memset(&client,0,sizeof(client));
	client.sin_family = AF_INET;
	client.sin_port = htons(9000);
	client.sin_addr.s_addr = INADDR_ANY;
	if (connect(c_sock,(struct sockaddr*)&client,sizeof(client))==-1)
	{
		printf("connection not established\n");
		return 0;
	}
	recv(c_sock,buf,sizeof(buf),0);
	printf("message from server :%s\n",buf);
	printf("Enter the first number\n");
	scanf("%s",&n);
	send(c_sock,n,sizeof(n),0);
	printf("Enter the second number\n");
	scanf("%s",&n1);
	send(c_sock,n1,sizeof(n1),0);
	printf("Enter the required operation\n");
	scanf("%s",&o);
	send(c_sock,o,sizeof(o),0);
	recv(c_sock,res,sizeof(res),0);
	printf("Result = %s \n",res );
	close(c_sock);
	return 0;


}