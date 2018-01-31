#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include "ts.h"

void gotoxy(int x,int y)
{
    printf("%c[%d;%df",0x1B,y,x);
}

int main(int argc, char *argv[])
{
    system("clear");
    int sockfd = 0;
    
    struct sockaddr_in serv_addr;

    /* Create a socket first */
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0))< 0)
    {
        printf("\n Error : Could not create socket \n");
        return 1;
    }

    /* Initialize sockaddr_in data structure */
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(5000); // port
    //char ip[50] = "18.220.8.85";
    char ip[50] = "127.0.0.1";

    serv_addr.sin_addr.s_addr = inet_addr(ip);

    /* Attempt a connection */
    if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr))<0)
    {
        printf("\n Error : Connect Failed \n");
        return 1;
    }

    printf("Connected to ip: %s : %d\n",inet_ntoa(serv_addr.sin_addr),ntohs(serv_addr.sin_port));

   	 /* Create file where data will be stored */
    
    char buff[2048];
    
    while (1)
    {
        read(sockfd, buff, 2048);
        printf("%s\n",buff);
        
        fgets(buff, 2048, stdin);
        char k = buff[0];
        write(sockfd, buff, 1);
        if (k == 1)
            continue;
        if (k == 'q' || k == 'Q')
            break;
        processCmdClient(k, sockfd, buff);
    }
    
	
    return 0;
}
