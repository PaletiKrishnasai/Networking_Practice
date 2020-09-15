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

char questions[][100] = {"What is 5+1? A)6 B)2 C)1 D)4", "What is 2+1? A)6 B)3 C)1 D)4", "What is 1+3? A)6 B)2 C)1 D)4", "What is 21x2? A)62 B)22 C)1 D)42", "what is n-n? A)6 B)2 C)0 D)4"};
char answers[][25] = {"A", "B", "D", "D", "C"};
int no_of_ques = 5;

int numb_clients=0;

struct ctime
{
	char name[20];
	time_t answertime; //ld
};

/*Change 1*/
time_t mintime=51; //1 s greater than max timeout

char minname[20]=" ";



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

	time_t avg_time;
	int correct;
	int wrong;
	int late;
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

int GetCurTimeInSec()
{
	int sec = GetCurTime("seconds", 0) + 60*GetCurTime("minutes", 0) + 3600*GetCurTime("hour", 0);
	return sec;
}

int main()
{

	FILE *fp;
	fp=fopen("tmp.txt","w");
	fprintf(fp,"%d",0);
	fclose(fp);
	
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

						client.avg_time = 0;
						client.correct = 0;
						client.wrong = 0;
						client.late = 0;

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

						

						int time_s = 0, time_e = 0; //time in seconds and microseconds
						int time_to_ans = 0;

						char correct[15] = "Correct!";
						char wrong[100] = "Better Luck Next Time...";
						char late[15] = "Too Late...";

						char ans[200];

						char over[2];
						over[0] = '0';
						over[1] = '\0';

						for(int i=0;i<no_of_ques;i++)
						{
							time_t start,end;
							
							send(s_server_child, over, sizeof(over), 0);
							printf("\nQuestion for %s: %s\n", client.name, questions[i]);
							
							send(s_server_child, questions[i], sizeof(questions[i]), 0);
							
							time(&start);
							
							recv(s_server_child, ans, sizeof(ans), 0);
							
							time(&end);
							time_t diff=end-start;
							
							client.avg_time += diff;/// no_of_ques; //Need to review

							if(strcmp(ans,"-666")==0)
							{
								client.late++;
								printf("\nAnswer entered by %s: %s -- Late Answer (%ld sec to answer)\n", client.name, ans, diff);
								send(s_server_child, late, sizeof(late), 0);
							}
							else if(strcmp(answers[i], ans) == 0)
							{
								client.correct++;
								printf("\nAnswer entered by %s: %s -- Correct (%ld sec to answer)\n", client.name, ans, diff);
								send(s_server_child, correct, sizeof(correct), 0);
							}
							else 
							{
								client.wrong++;
								printf("\nAnswer entered by %s: %s -- Wrong -- Expected %s\n", client.name, ans, answers[i]);
								send(s_server_child, wrong, sizeof(wrong), 0);
								break;

							}
						}
						printf("\nOVER\n");
						over[0] = '1';
						over[1] = '\0';
						send(s_server_child, over, sizeof(over), 0);

						char result[25];
						sprintf(result, "%dc%dw%dl%dq%ldt", client.correct, client.wrong, client.late, no_of_ques,client.avg_time);
						send(s_server_child, result, sizeof(result), 0);
						
						/*if(client.correct==5) //if all correct answers
						{
							int temp;
							fp=fopen("tmp.txt","a");
							fseek(fp,0,SEEK_START);
							fscanf()
						}*/
						
						if(client.avg_time<mintime)
						{
							mintime=client.avg_time;
							strcpy(minname,client.name);
						}
						
						close(s_server_child);
						printf("Do you want to exit (Press 1 to exit): "); //0 to exit
						scanf("%d", &ex);
						if(ex==1)
						{
							break;
						}	
						//exit(0);
					}
				}
			}
		}

		close(s_server);
		close(s_socket);

		
		printf("Winner of the game is %s with time %ld seconds",minname,mintime);
		mintime=51;
		strcpy(minname, " ");
		//return 0;
	}

	return 0;
}
