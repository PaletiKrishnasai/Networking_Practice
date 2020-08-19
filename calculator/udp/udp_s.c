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
	int s_sock,a,b,c;
	double value;
	//char buf[20]="hello client";
	char n[20],n1[20],o[20],res[20];
	s_sock = socket(AF_INET, SOCK_DGRAM, 0);
	struct sockaddr_in server ,client;
	server.sin_family = AF_INET;
	server.sin_port = htons(9009);
	server.sin_addr.s_addr=INADDR_ANY;
	bind(s_sock,(struct sockaddr*)&server,sizeof(server));
	socklen_t client_len;
	client_len=sizeof(client);
	
	printf("Please enter first number Client\n");
	recvfrom(s_sock, n, sizeof(n), 0, (struct sockaddr*)&client, &client_len);
	a=atoi(n);

	printf("Please enter first number Client\n");
	recvfrom(s_sock, n1, sizeof(n1), 0, (struct sockaddr*)&client, &client_len);
	b=atoi(n1);

	printf("operations available \n1.ADD\n2.SUBTRACT\n3.MULTIPLY\n4.DIVIDE\n5.MOD\n");
	recvfrom(s_sock, o, sizeof(o), 0, (struct sockaddr*)&client, &client_len);
	c=atoi(o);

	switch(c)
		{
			case 1: value = a+b;break;
			case 2: value = a-b;break;
			case 3: value = a*b;break;
			case 4: value = a/b;break;
			case 5: value = a%b;break;
			default : printf("Invalid operation\n"); 

		}
	sprintf(res,"%lf",value);
	sendto(s_sock, res, sizeof(res), 0, (struct sockaddr*)&client, sizeof(client));



close(s_sock);
return 0;

}