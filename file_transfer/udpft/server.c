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
	/*int s_sock;

int servercreate (int port)
{
	s_sock = socket(AF_INET,SOCK_DGRAM,0);
	struct sockaddr_in server , client;
	memset(&server,0,sizeof(server));
	memset(&client,0,sizeof(client));
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	server.sin_addr.s_addr = INADDR_ANY;

	if(bind(s_sock,(struct sockaddr*)&server,sizeof(server))!=-1)
	{
		printf("Server up and running...\n");

		socklen_t add;
		add = sizeof(client);
		
		return 0;
	}
	else
	{
		printf("Error in bind....\n");
		return 1;
	}
}
*/
int main()
{
	int s_sock;
	s_sock = socket(AF_INET,SOCK_DGRAM,0);
	struct sockaddr_in server , client;
	memset(&server,0,sizeof(server));
	memset(&client,0,sizeof(client));
	server.sin_family = AF_INET;
	server.sin_port = htons(8080);
	server.sin_addr.s_addr = INADDR_ANY;
	socklen_t add;
		add = sizeof(client);

	if(bind(s_sock,(struct sockaddr*)&server,sizeof(server))!=-1)
	{
		printf("Server up and running...\n");

		
		
		
	}
	else
	{
		printf("Error in bind....\n");
		
	}
		printf("Server started \n");

		char filename[100];
		char filename_ext[100];
		char ext[20];
		char buf[1];
		char endoffile[1]="0";

		recvfrom(s_sock,filename,sizeof(filename),0,(struct sockaddr*)&client, &add);
		recvfrom(s_sock,ext,sizeof(ext),0,(struct sockaddr*)&client, &add);

		if(strlen(ext) == 0) return 0;

		strcpy(filename_ext,filename);
		strcat(filename_ext,"_recv.");
		strcat(filename_ext,ext);

		FILE *fp;
		fp = fopen(filename_ext,"w");

		int flag = 0;
		while (flag ==0)
		{
			//printf("receiving!\n");
			recvfrom(s_sock,endoffile,sizeof(endoffile),0,(struct sockaddr*)&client, &add);
			if(endoffile[0]=='1') flag =1;
			else
			{
				recvfrom(s_sock,buf,sizeof(buf),0,(struct sockaddr*)&client, &add);
				fprintf(fp,"%c",buf[0] );
			}
		}
		printf("File end\n");
		fclose(fp);

		printf("File Recv successful\n");
	
	close(s_sock);
	return 0;


}
