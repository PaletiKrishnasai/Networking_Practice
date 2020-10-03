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

int c_sock;

int ClientCreate(int port,int anyip,char IPADDR[])
{
    c_sock = socket(AF_INET,SOCK_STREAM,0);
    struct sockaddr_in client;
    memset(&client,0,sizeof(client));
    client.sin_family = AF_INET;
    client.sin_port = htons(port);
    if(anyip == 0) 
    client.sin_addr.s_addr = inet_addr(IPADDR);
    else
    client.sin_addr.s_addr = INADDR_ANY;
    if(connect(c_sock , (struct sockaddr*)&client,sizeof(client)) == -1)
    {
        printf("\nConnection FAILED\n");
        return 1;
    }
    else
    {
        return 0;
    }
}

int main()

{
    int exitloop = 0 ;
    while(exitloop == 0)
    {
        char ipaddr[30] = "127.0.0.1";
        int port = 8080;
        int error = ClientCreate(port,0,ipaddr);
        if(error == 1)
        {
            close(c_sock);
            printf("\nConnection FAILED\n");
        }
        else
        {
            char message[512];
            printf("\nEnter message to send : ");
            scanf("%s",message);

            char acknowledgement[2];
            acknowledgement[0] = '1';
            acknowledgement[1] = '\0';

            char buffer[2];
            buffer[0] = '1';
            buffer[1] = '\0';

            char exit[2];
            exit[0] = '0';
            exit[1] = '\0';
//packet sending simulation with each byte as a packet.
            int index =0;
            while(index < strlen(message))
            {
                printf("Sending : %c \n",message[index]);
                buffer[0] = message[index];
                send(c_sock,exit,sizeof(exit),0); // synchronising the server loop 
                send(c_sock,buffer,sizeof(buffer),0); // byte by byte
                recv(c_sock,acknowledgement,sizeof(acknowledgement),0); // recv acknowledgement from server
                printf("Acknowedgement : %s\n",acknowledgement); 
                if(acknowledgement[0] == '1')
                index++;  // go to next byte if acknowledgement is 1  , else resend the same byte/packet.
            }
            exit[0] = '1';
            send(c_sock,exit,sizeof(exit),0);
        }

        printf("\n0 - continue\n1 - exit\n");
        scanf("%d",&exitloop);

       /* if(exitloop == 1)
        break;
        else continue;
        */

        close(c_sock);
        
    }
return 0;
}