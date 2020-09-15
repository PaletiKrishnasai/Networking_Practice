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

int GetCurTime(char returnval[], int display)
{
	time_t t;
	struct tm * timeinfo;
	time(&t);
	timeinfo = localtime(&t);

	int year = timeinfo->tm_year + 1900;
	int month = timeinfo->tm_mon + 1;
	int date = timeinfo->tm_mday;
	int hour = timeinfo->tm_hour;
	int minutes = timeinfo->tm_min;
	int seconds = timeinfo->tm_sec;

	if(display == 1) printf("\nCurTime: %d:%d:%d - %d/%d/%d\n", hour, minutes, seconds, date, month, year);

	if(strcmp(returnval, "year") == 0) return year;
	else if(strcmp(returnval, "month") == 0) return month;
	else if(strcmp(returnval, "date") == 0) return date;
	else if(strcmp(returnval, "hour") == 0) return hour;
	else if(strcmp(returnval, "minutes") == 0) return minutes;
	else if(strcmp(returnval, "seconds") == 0) return seconds;
	else return -1;
}

int main()
{

	//GetCurTime();

	int ex2 = 0; //denotes an exit flag

	while(ex2 == 0)
	{
		char name[50];
		printf("\n\nEnter Name: ");
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
			
			char ques[200];
			char ans[200];
			char reply[100];
			

			printf("[+] Server Connected\n\n");

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
					recv(c_socket, ques, sizeof(ques), 0); //receive the questions from the server
					printf("\nQuestion: %s\n", ques);

					printf("Enter Answer: ");
					fflush(stdin);
					
					int iResult; //Denote timeout
					struct timeval tv;
			      	fd_set rfds;
			      	FD_ZERO(&rfds);
			      	FD_SET(0, &rfds);

			     	tv.tv_sec = 10; //timeout after 10 seconds
			      	iResult = select(1, &rfds, NULL, NULL, &tv); //use select() function of Linux
			      	if(iResult == 0)
			      	{
			          printf("TimeOut\n");
			          strcpy(ans,"-666");
			          exit(0);
			      	} 
			      	else
			      	{
			         	FD_ISSET(0,&rfds);
			         	scanf(" %[^\n]",ans); //use a regular expression for inputs

			      	} 
				send(c_socket, ans, sizeof(ans), 0);

				recv(c_socket, reply, sizeof(reply), 0);
				printf("\n%s\n", reply);
				if(strcmp(reply,"Better Luck Next Time...")==0)
				{
					close(c_socket);
					return 0;
				}
					
				}
			}
			char result[25];
			recv(c_socket, result, sizeof(result), 0);
			printf("\nResult: \nCorrect: ");
			for(int i=0;i<strlen(result);i++)
			{
				
				if(result[i] == 'c')
				{
					printf("\nWrong: ");
				}
				else if(result[i] == 'w')
				{
					printf("\nLate: ");
				}
				else if(result[i] == 'l')
				{
					printf("\nTotal no of questions: ");
				}
				else if(result[i] == 'q')
				{
					printf("\n Total time: ");
				}
				else if(result[i] == 't')
				{
					printf("\n");
				}
				else
					printf("%c", result[i]);
			}
		}

		printf("Do you want to exit (Enter 1 to exit): "); //0 to exit
		scanf("%d", &ex2);

		
	}
	close(c_socket);
	return 0;
}
