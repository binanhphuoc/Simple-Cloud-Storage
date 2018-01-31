#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <pthread.h>
#include "ts.h"

struct sockaddr_in c_addr;
char fname[100];


void* respondToClient(void *arg)
{
    
    char buff[2048];
    int connfd=*(int *)arg;
    
    printf("Connection accepted and id: %d\n",connfd);
    printf("Connected to Clent: %s:%d\n",inet_ntoa(c_addr.sin_addr),ntohs(c_addr.sin_port));
    
    shellService("binanhphuoc", connfd);
        
    
    printf("Closing Connection for id: %d\n",connfd);
    close(connfd);

    shutdown(connfd,SHUT_RDWR);

    sleep(2);
    return NULL;
}

int main(int argc, char *argv[])
{
    int connfd = 0,err;
    pthread_t tid;
    struct sockaddr_in serv_addr;
    int listenfd = 0,ret;
    char sendBuff[1025];
    int numrv;
    socklen_t clen=0;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if(listenfd<0)
    {
        printf("Error in socket creation\n");
        exit(2);
    }

    printf("Socket retrieve success\n");

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(5000);

    ret=bind(listenfd, (struct sockaddr*)&serv_addr,sizeof(serv_addr));
    /*
    if(ret<0)
    {
      printf("Error in bind\n");
      exit(2);
    }
     */

    if(listen(listenfd, 10) == -1)
    {
        printf("Failed to listen\n");
        return -1;
    }

    /*
    if (argc < 2)
    {
        printf("Enter file name to send: ");
        gets(fname);
    }
    else
    strcpy(fname,argv[1]);
*/
    
    while(1)
    {
        clen=sizeof(c_addr);
        printf("Waiting...\n");
        connfd = accept(listenfd, (struct sockaddr*)&c_addr,&clen);
        if(connfd<0)
        {
            printf("Error in accept\n");
            continue;
        }
        
        /*
        err = (int) pthread_create(&tid, NULL, &respondToClient, &connfd);
        
        if (err == 1)
        {
            printf("\ncan't create thread :[%s]", strerror(err));
            continue;
        }*/
        
        pid_t pid;
        pid = fork();
        if (pid == 0)
        {
            respondToClient(&connfd);
        }
        else if (pid < 0)
        {
            printf("\ncan't create process\n");
            continue;
        }
    }

    close(connfd);
    return 0;
}
