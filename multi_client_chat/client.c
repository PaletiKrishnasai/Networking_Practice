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

int c_socket;

char arr[100];
int arr_size;

int ClientCreate(int port, int anyip, char IPADDR[])// Return 1 for error
{
	c_socket = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in client;
	memset(&client, 0, sizeof(client));
	client.sin_family = AF_INET;
	client.sin_port = htons(port);
	if(anyip == 0) client.sin_addr.s_addr = inet_addr(IPADDR);
	else client.sin_addr.s_addr = INADDR_ANY;
	if(connect(c_socket, (struct sockaddr *)&client, sizeof(client)) == -1)
	{
		printf("Connection Issue.\n");
		return 1;
	}
	else return 0;
}


int main()
{

	//GetCurTime();
	char buffer[1000]="Enter exit to stop";
	int ex2 = 0; //denotes an exit flag

	while(ex2 == 0)
	{
		char name[50];
		printf("\nEnter Name: ");
		scanf("%s", name);

		char ipaddr[20]="127.0.0.1";
		/*printf("Enter IP Address: ");
		scanf("%s", ipaddr);*/

		int port;
		printf("Enter Port: ");
		scanf("%d", &port);

		int error = ClientCreate(port, 0, ipaddr);
		if(error == 1)
		{
			close(c_socket);
			printf("Connection Issue.\n");
		}
		else 
		{

			int recv_mode = 0;

			char port_str[20];

			int i=0;
			int p = port;
			while(p > 0) //converting port to a string
			{
				port_str[i] = (char)(48 + p%10);
				i++;
				p = p/10;
			}

			port_str[i] = '\0';	

			send(c_socket, name, sizeof(name), 0);
			send(c_socket, ipaddr, sizeof(ipaddr), 0);
			send(c_socket, port_str, sizeof(port_str), 0);
			//send the name, ipaddr and port to the server
			
			printf("[+] Server Connected\n");

			int exloop = 0;  //exit flag for loop
			while(exloop == 0)
			{
				char over[2]; //to stop the server from repeating 
				recv(c_socket, over, sizeof(over), 0);
				if(over[0] == '1')
				{
					exloop = 1;
				}
				else 
				{
					
				printf("%s\n", buffer);
				printf("Enter message to send\n");	
				fgets(buffer,1000,stdin);
				send(c_socket, buffer, sizeof(buffer), 0);
				
				usleep(1000);
				
//				printf("Message from server: ");
				recv(c_socket, buffer, sizeof(buffer), 0);
//				printf("\n%s\n", buffer);
				
				if(strncmp(buffer,"exit",4)==0)
				{
					break;	
				}
				
				}
			}
			
		}

		printf("Do you want to exit (Enter 1 to exit): "); //0 to exit
		scanf("%d", &ex2);

		
	}
	close(c_socket);
	return 0;
}
