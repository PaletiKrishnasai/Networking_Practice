#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<sys/time.h>
#include<netinet/in.h>
#include<string.h>
#include<unistd.h>
#include <arpa/inet.h>
#include<time.h>


int s_socket, s_server; 
/*
s_socket is server socket
s_server is a socket holding details of the client
*/
char clientip[20];

int ServerCreate(int port)// Return 1 for error
{
	s_socket = socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in server, other;
	memset(&server, 0, sizeof(server));
	memset(&other, 0, sizeof(other));

	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	server.sin_addr.s_addr = INADDR_ANY;

	
	if(bind(s_socket, (struct sockaddr*)&server, sizeof(server)) != -1)
	{
		printf("Server Running.....\n");
		listen(s_socket, 5);

		return 0;
	}
	else
	{
		printf("\nError in Bind.\n");
		return 1;
	} 
}

int AcceptNewClient()
{
	struct sockaddr_in other;
	memset(&other, 0, sizeof(other));
	socklen_t add = sizeof(other);

	s_server = accept(s_socket, (struct sockaddr*)&other, &add);
	if(s_server == -1) return 1;
	else 
	{	
		strcpy(clientip, inet_ntoa(other.sin_addr));
		printf("\n[+] Conection accepted from %s,%d\n",inet_ntoa(other.sin_addr),ntohs(other.sin_port));
		return 0;
	}
}

struct Client
{
	char name[50];
	char ipaddr[20];
	char port_str[20];

	int s_server;

};

void DisplayConnectedClients(struct Client clients[], int clients_size)
{
	printf("\nConnected Clients: ");
	for(int i=0;i<clients_size;i++)
	{
		printf("%s, ", clients[i].name);
	}
	printf("\n");
}

int main()
{
	char buffer[1000] = "Enter exit to stop";
	char over[2];

	over[0] = '0';
	over[1] = '\0';
	
	int ex = 0;

	while(ex == 0)
	{
		int port;
		printf("Enter Port: ");
		scanf("%d", &port);

		int error = ServerCreate(port);
		if(error == 1)
		{
			close(s_server);
			close(s_socket);
			printf("Server Issue.\n");
			return 0;
		}
		else 
		{
			printf("\nServer Waiting...\n");


			while(1)
			{
				printf("Ready to accept new client...\n");
				int accepterror = AcceptNewClient();
				if(accepterror == 0)
				{

					int pid = fork();
					if(pid > 0)
					{
						//return 0;
						continue;
					} 
					else if(pid == 0)
					{
						struct Client client;

						int s_server_child = s_server;
						client.s_server = s_server_child;
						strcpy(client.ipaddr, clientip);

						char name[50];
						char ipaddr[20];
						char port_str[20];
						

						recv(s_server_child, name, sizeof(name), 0);
						recv(s_server_child, ipaddr, sizeof(ipaddr), 0);
						recv(s_server_child, port_str, sizeof(port_str), 0);
						

						strcpy(client.name, name);
						
						strcpy(client.port_str, port_str);

						

						while(1)
						{
//							printf("%s",buffer);
							
							send(s_server_child, over, sizeof(over), 0);
							
							
/*
Working:
The socket messages will act like a stack
When c1 sends a message and then c2 sends a message, the reply from server will go to c2 and then c1 can receive replies
This pattern continues till we end execution
So, this acts as a chat program with immediate reply i.e. the latest message gets the priority in replies received
The earliest message receives the replies last
*/
							usleep(1000); //to account for any delays in transmission
							recv(s_server_child, buffer, sizeof(buffer), 0);
							
							printf("\nMessage from %s: %s\n", client.name, buffer);
							
							
							printf("Message for %s: ", client.name);
							fgets(buffer,1000,stdin);
							send(s_server_child, buffer, sizeof(buffer), 0);
							
							if(strncmp(buffer,"exit",4)==0)
							{
								break;
							}
							
						}
					
						printf("\nOVER\n");
						over[0] = '1';
						over[1] = '\0';
						send(s_server_child, over, sizeof(over), 0);
						
						close(s_server_child);
					}
						printf("Do you want to exit (Press 1 to exit): "); //0 to exit
						scanf("%d", &ex);
						if(ex==1)
						{
							break;
						}	
						
					}
				}
			}
		}

		close(s_server);
		close(s_socket);

	return 0;
}

