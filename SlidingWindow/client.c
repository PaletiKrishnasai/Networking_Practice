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

/*  Sliding window - Selective Repeat - to retransmit lost packets based
on acknowledgement */

int c_sock;

int ClientCreate(int port, int anyip, char IPADDR[])
{
    c_sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in client;
    memset(&client, 0, sizeof(client));
    client.sin_family = AF_INET;
    client.sin_port = htons(port);
    if (anyip == 0)
        client.sin_addr.s_addr = inet_addr(IPADDR);
    else
        client.sin_addr.s_addr = INADDR_ANY;
    if (connect(c_sock, (struct sockaddr *)&client, sizeof(client)) == -1)
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
    int exitloop = 0;
    while (exitloop == 0)
    {
        
        char ipaddr[30] = "127.0.0.1";
        int port = 8080;
        int error = ClientCreate(port, 0, ipaddr);
        int window = 0;
        printf("\nEnter the window size: \n");
        scanf("%d", &window);
        if (error == 1)
        {
            close(c_sock);
            printf("\nConnection FAILED\n");
        }
        else
        {
            char message[512] = {0};
            printf("Enter message to send : ");
            scanf("%s", message);

            char acknowledgement[2];
            acknowledgement[0] = '1';
            acknowledgement[1] = '\0';

            char buffer[2];
            buffer[0] = '1';
            buffer[1] = '\0';

            char exit[2];
            exit[0] = '0';
            exit[1] = '\0';

            int index = 0;
// frame by frame simulation
            while(index<strlen(message))
            {
                send(c_sock,exit,sizeof(exit),0);
                printf("\n");
                for(int i=0;i<window;i++)
                {
                    
                    printf("Sending : %c \n", message[index+i]);
                    buffer[0] = message[index+i];
                    send(c_sock, buffer, sizeof(buffer), 0); // frame by frame

                }
                int check = -1;
                //  receive acknowledgement frame by frame
                //  if a certain packet in a frame is lost(ack = 0) , then the next frame starts from that point.
                for(int i=0;i<window;i++)
                {
                    recv(c_sock,acknowledgement,sizeof(acknowledgement),0);
                    printf("Acknowedgement : %s\n", acknowledgement);
                  
                    if(acknowledgement[0]=='0')
                    {
                       
                        check = i;
                        i = window;

                    }
                    
                }
                if(check == -1)
                {
                    // when acknowledgement is not given , it resends form that packet onwards in a frame
                    index = index + window;

                }
                else
                {
                    index = index + check;
                }
                
            }
            exit[0] = '1';
            send(c_sock,exit,sizeof(exit),0);
        }
        printf("\n0 - continue\n1 - exit\n");
        scanf("%d", &exitloop);
    }
    return 0;
}