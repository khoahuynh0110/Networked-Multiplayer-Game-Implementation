//Tran Anh Khoa Huynh
//TXH220015

/* make up unique pipename for client
 * send it to the server (public pipe)
 * get another unique pipename from the server
 * repeat
 * 	read a line including starred word from the client pipe
 *  display that line to the user 
 *  check whether game is over?
 *  get the user's guess letter
 *  send to the server using server pipe
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#define MAXLEN 1000


int main(int argc, char *argv[]) {
    // Check if the server FIFO name is provided as a command line argument
    if (argc !=2) {
                puts("Usage: gclient <server-fifo-name>");
                exit(1);
        }

        // Create a unique client FIFO name based on the user's name and process ID
        char clientfifo[MAXLEN];
        sprintf(clientfifo, "/tmp/%s-%d", getenv("USER"), getpid());
        mkfifo(clientfifo, 0600); // Create the client FIFO with the specified permission
        chmod(clientfifo, 0622); // Change the permission of the client FIFO

        // Open the server FIFO for writing and write the client FIFO name to it
        FILE *fp = fopen(argv[1], "w");
        fprintf(fp, "%s\n", clientfifo);
        fclose(fp);

        // Open the client FIFO for reading
        FILE *clientfp = fopen(clientfifo, "r");

        // Read the server FIFO name from the client FIFO
        char serverfifo[MAXLEN];
        fscanf(clientfp, "%s", serverfifo);

        // Read the first message from the server
        char line[MAXLEN];
        fgets(line, MAXLEN, clientfp);

        // Open the server FIFO for writing and start the game loop
        FILE *serverfp = fopen(serverfifo, "w");
        while(1) {
                fgets(line, MAXLEN, clientfp); // Read the next message from the server
                //This will help get rid of \n
                char *cptr = strchr(line, '\n');
                if(cptr) {
                        *cptr = '\0';
                }       
                if(strstr(line, "Enter")) { // If the message is asking for a letter
                        printf("%s", line); // Print the message
                        char buf[10];
                        scanf("%s", buf); // Read a letter from the user

                        fprintf(serverfp, "%c\n", buf[0]); // Write the letter to the server FIFO
                        fflush(serverfp); // Flush the output stream
                } else if(strstr(line, "You missed")) { // If the message indicates that the user missed a letter
                        printf("%s\n", line); // Print the message
                        break; // Exit the game loop
                } else { // If the message is a regular game message
                        printf("%s\n", line); // Print the message
                }
        }

        fclose(clientfp); // Close the client FIFO
        unlink(clientfifo); // Remove the client FIFO from the file system
        fclose(serverfp); // Close the server FIFO
        exit(0); // Exit the program
}


