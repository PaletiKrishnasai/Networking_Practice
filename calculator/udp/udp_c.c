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
	int c_sock;
	char buf[40];
	c_sock = socket(AF_INET, SOCK_DGRAM, 0);
	struct sockaddr_in client;
	socklen_t *client_len;
	client_len = sizeof(client);
	client.sin_family = AF_INET;
	client.sin_port = htons(9009);
	client.sin_addr.s_addr = INADDR_ANY;

		printf("Enter first number \n");
		fflush(stdin);
		fgets(buf, 40, stdin);
		sendto(c_sock, buf, sizeof(buf), 0, (struct sockaddr*)&client, sizeof(client));
		
		printf("Enter second number\n");
		fflush(stdin);
	    fgets(buf, 40, stdin);
		sendto(c_sock, buf, sizeof(buf), 0, (struct sockaddr*)&client, sizeof(client));

		printf("Enter operator\n");
		fflush(stdin);
	    fgets(buf, 40, stdin);
		sendto(c_sock, buf, sizeof(buf), 0, (struct sockaddr*)&client, sizeof(client));

		recvfrom(c_sock, buf, sizeof(buf), 0, (struct sockaddr*)&client, client_len);
		printf("Result: %s\n",buf);

close(c_sock);
return 0;
}