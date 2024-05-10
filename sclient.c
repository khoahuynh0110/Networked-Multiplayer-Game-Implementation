//Tran Anh Khoa Huynh
//This program will run the programs across the internet
//This is guessing game version 1 from assignment 3

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

int main(int argc, char *argv[])
{
    int sockfd = 0, n = 0;
    char recvBuff[1024];
    struct sockaddr_in serv_addr;

    if(argc != 3) {
        printf("\nUsage: %s <ip of server> <port #> \n",argv[0]);
        return 1;
    }

    memset(recvBuff, '0',sizeof(recvBuff));
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\nError : Could not create socket \n");
        return 1;
    }

    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
        int port = atoi(argv[2]);
    serv_addr.sin_port = htons(port);

    if(inet_pton(AF_INET, argv[1], &serv_addr.sin_addr)<=0) {
        printf("\ninet_pton error occured\n");
        return 1;
    }

    if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
       printf("\nError : Connect Failed \n");
       return 1;
    }

        //read one line from the user
        //Send that to server
    char answer;
    printf("Connection Successful!\n");
    printf("Let think of a number between 1 and 100, then keep in mind! \n");
    while(1) {
        n = read(sockfd, recvBuff, sizeof(recvBuff));
        if(strstr(recvBuff, "Bye")) {
            printf("%s\n", recvBuff);
                        break;
        } else {
            fputs(recvBuff, stdout);
            scanf(" %c", &answer);
            write(sockfd, &answer, sizeof(char));
        }
    }

    if(n < 0) printf("\n Read error \n");

    return 0;
}
