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
	int s_sock , c_sock;
	double value;
	char buf[40]="Hello Client" ;
	char n[20],n1[20],o[20],res[20];
	s_sock = socket(AF_INET,SOCK_STREAM,0);
	struct sockaddr_in server , client;
	memset(&server,0,sizeof(server));
	memset(&client,0,sizeof(client));
	server.sin_family = AF_INET;
	server.sin_port = htons(9000);
	server.sin_addr.s_addr = INADDR_ANY;
	socklen_t add;
	if (bind(s_sock,(struct sockaddr*)&server,sizeof(server))==-1)
	{
		printf("Server Busy\n");
		return 0;
	}
	listen(s_sock,100);
	add = sizeof(client);
	c_sock = accept(s_sock,(struct sockaddr*)&client,&add); 
	send(c_sock,buf,sizeof(buf),0);
	printf("Please enter first number Client \n");
	recv(c_sock,n,sizeof(n),0);
	printf("Please enter the second number Client \n");
	recv(c_sock,n1,sizeof(n1),0);
	printf("operations available \n1.ADD\n2.SUBTRACT\n3.MULTIPLY\n4.DIVIDE\n5.MOD\n");
	recv(c_sock,o,sizeof(o),0);
	int a,b,c;
	a = atoi(n);//send and recv only pass arguments of string
	b = atoi(n1);
	c = atoi(o);
	switch(c)
		{
			case 1: value = a+b;break;
			case 2: value = a-b;break;
			case 3: value = a*b;break;
			case 4: value = a/b;break;
			case 5: value = a%b;break;
			default : printf("Invalid operation\n"); 

		}
		//itoa(value,res,10);//not able to declare!!!
		sprintf(res,"%lf",value);
		send(c_sock,res,sizeof(res),0);


	//printf("%s\n", );
	close(s_sock);

return 0;


}