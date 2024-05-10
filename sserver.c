//Tran Anh Khoa Huynh
//This program will run the programs across the internet
//This is guessing game version 1 from assignment 3

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h> 

void pexit(char *errmsg) {
	fprintf(stderr, "%s\n", errmsg);
	exit(1);
}


int main(int argc, char *argv[])
{
    int listenfd = 0, connfd = 0;
    struct sockaddr_in serv_addr; 

    char buffer[1025];
    time_t ticks; 

    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		pexit("socket() error.");
		
    memset(&serv_addr, '0', sizeof(serv_addr));
    memset(buffer, '0', sizeof(buffer)); 

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	int port = 4999;
	do {
		port++;
    	serv_addr.sin_port = htons(port); 
    } while (bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0);
	printf("bind() succeeds for port #%d\n", port);

    if (listen(listenfd, 10) < 0)
		pexit("listen() error.");

	int counter=0;
    int low = 1, high = 100, mid =0;
    char messagePrint[10000];
    char answer[3];

    while(1)
    {
       connfd = accept(listenfd, (struct sockaddr*)NULL, NULL); 
		counter++;
		printf("connected to client %d.\n", counter);
		if (fork() > 0)
			continue;
        int n; 

        
       
        while (low <= high) {
            mid = (low + high) / 2;
            sprintf(messagePrint,"How about %d (<,=,>)? ", mid);
            write(connfd, messagePrint, strlen(messagePrint) + 1);
            n = read(connfd, &answer, sizeof(char));

            if (answer[0] == '<') {
                high = mid - 1;
            } else if(answer[0] == '>') {
            low = mid + 1;
            }else if(answer[0] == '='){
	            break;
	        }
	        else {
	            printf("Client enter wrong character\n");
	        }
        }

        sprintf(messagePrint,"The number you are thinking of is %d\nIt was a good game,Bye!! ", mid);
        write(connfd, messagePrint, strlen(messagePrint) + 1);

        close(connfd);
		//user has quit - update the global arrays?
		
		exit(0); //this is child server process. It is done!
    }
}
