#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>


	int s_sock , c_sock ;

int servercreate (int port)
{
	s_sock = socket(AF_INET,SOCK_STREAM,0);
	struct sockaddr_in server , client;
	memset(&server,0,sizeof(server));
	memset(&client,0,sizeof(client));
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	server.sin_addr.s_addr = INADDR_ANY;

	if(bind(s_sock,(struct sockaddr*)&server,sizeof(server))!=-1)
	{
		printf("Server up and running...\n");
		listen(s_sock,10);
		socklen_t add;
		add = sizeof(client);
		c_sock = accept(s_sock,(struct sockaddr*)&client,&add);
		return 0;
	}
	else
	{
		printf("Error in bind....\n");
		return 1;
	}
}

int main()
{
	/*
	int s_sock , c_sock ;
	s_sock = socket(AF_INET,SOCK_STREAM,0);
	struct sockaddr_in server , client;
	memset(&server,0,sizeof(server));
	memset(&client,0,sizeof(client));
	server.sin_family = AF_INET;
	server.sin_port = htons(8087);
	server.sin_addr.s_addr = INADDR_ANY;
	if(bind(s_sock,(struct sockaddr*)&server,sizeof(server))!=-1)
	{
		printf("Server up and running...\n");
		listen(s_sock,10);
		printf("server u b for assets1\n");
		socklen_t add;
		printf("server u b for assets2\n");
		add = sizeof(client);
		printf("server u b for assets3\n");
		c_sock = accept(s_sock,(struct sockaddr*)&client,&add);
		printf("server u b for assets4\n");
	}
	else
	{
		printf("Error in bind....\n");
		
	}*/


	int check = servercreate(8080);
	if (check==1)
	{
		close(c_sock);
		close(s_sock);
		printf("Binding problem ...\n");
		return 0;
	}
	else
	{
		printf("Server started \n");

		char filename[100];
		char filename_ext[100];
		char ext[20];
		char buf[1];
		char endoffile[1]="0";

		recv(c_sock,filename,sizeof(filename),0);
		recv(c_sock,ext,sizeof(ext),0);

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
			recv(c_sock,endoffile,sizeof(endoffile),0);
			if(endoffile[0]=='1') flag =1;
			else
			{
				recv(c_sock,buf,sizeof(buf),0);
				fprintf(fp,"%c",buf[0] );
			}
		}
		printf("File end\n");
		fclose(fp);

		printf("File Recv successful\n");
	}
	close(c_sock);
	close(s_sock);
	return 0;
}