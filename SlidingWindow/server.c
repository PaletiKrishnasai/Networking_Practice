#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

int s_sock, c_sock;

int ServerCreate(int port)
{
    s_sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server, other;
    memset(&server, 0, sizeof(server));
    memset(&other, 0, sizeof(other));
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = INADDR_ANY;

    if (bind(s_sock, (struct sockaddr *)&server, sizeof(server)) == -1)
    {
        printf("\nError in Bind\n");
        return 1;
    }
    else
    {
        printf("\nServer Running...\n");
        listen(s_sock, 5);
        return 0;
    }
}

int AcceptNewClient()
{
    struct sockaddr_in other;
    memset(&other, 0, sizeof(other));
    socklen_t add = sizeof(other);

    c_sock = accept(s_sock, (struct sockaddr *)&other, &add);
    if (c_sock == -1)
        return 1;
    else
    {
        printf("\nConnection Accepted\n");
        return 0;
    }
}

int main()

{
    int exitloop = 0;

    while (exitloop == 0)
    {
        int port = 8080;
        int accept;
        int error;
        int window=0;
        
        error = ServerCreate(port);
        accept = AcceptNewClient();
        printf("\nEnter the window size : \n");
        scanf("%d", &window);
        if (error == 1 || accept == 1)
        {
            close(c_sock);
            close(s_sock);
            printf("\nProgram fails at create and accept\n");
            return 0;
        }
        else
        {
            printf("\nServer Waiting...\n");

            char message[512];
            int index = 0;

            char acknowledgement[2];
            acknowledgement[0] = '1';
            acknowledgement[1] = '\0';

            char temp[2]; // temporary ack variable for easier debugging
            char buffer[2];
            char exit[2];
            int loop = 0;
            

            while(loop == 0)
            {
                recv(c_sock, exit, sizeof(exit), 0);
                if (exit[0] == '1')
                {
                    loop = 1;
                }

                else
                {
                    for(int i=0;i<window;i++)
                    {
                        recv(c_sock,buffer,sizeof(buffer),0);
                        printf("packet : %s\n", buffer);
                        message[index] = buffer[0];
                        index += 1;
                    }
                    for(int i=0;i<window;i++)
                    {
                        printf("Enter acknowledgement : ");
                        scanf("%s", temp);
                        acknowledgement[0] = temp[0];
                        send(c_sock,acknowledgement,sizeof(acknowledgement),0);

                        if(acknowledgement[0]=='0')
                        {
                            index = index - window + i ;
                            i = window;
                        }
                    }
                }
                
            }
            printf("Message : %s\n",message);

        }
        close(c_sock);
        close(s_sock);
        printf("\n0 - continue\n1 - exit\n");
        scanf("%d", &exitloop);
    }
    

    return 0;
}