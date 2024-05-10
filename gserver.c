//Tran Anh Khoa Huynh
//TXH220015


/*
 * qserver4.c in a10 is a good one with lot of relevant code!
 *
 * read dictionary file to array of words & get ready to play the hangman!
if you are using fgets() to read the word
	  cptr = strchr(line, '\n');
	  if (cptr)
	  	*cptr = '\0';
 However, since we are dealing with words, you can use fscanf(...,"%s", ...) instead!

 * wait for a request to come in (client specifies unique pipename)
 * select a random word using rand()
 * fork() to create a child to handle this client! (dedicated server process for that client)
 * fork() enables games to proceed in parallel. Parent returns to wait for new client requests
 * respond with another unique server-side pipename 
 *
 * send a bunch of stars (indicating the word length)
 * for each guess the client sends in, respond with a message 
 * and send updated display word.
 *
 * Whenever you send strings through named pipes, 
 * terminate with '\n' and also do fflush() for that data to make it to other side without getting stuck
 * in buffers.
 *
 * open public fifo
 * while (fgets()) {
 * }
 *
 *


srand(....);

wait for a client connection
rand() to select a word
fork()
	child process:
*/
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>

#define MAXWORD 1000000
#define MAXLEN 1000

char words[MAXWORD][MAXLEN]; // List of words from message
int numWords = 0;

int main() {
        char line[MAXLEN];
        char clientInput[MAXLEN]; // Prompt the clientInput
        
        //Open the file and store all word to the list
        FILE *file = fopen("dictionary.txt", "r");
        if(!file) {
                puts("dictionary.txt cannot be opened for reading.");
                exit(1);
        }
        
        while(fscanf(file, "%s", words[numWords]) == 1) {
                numWords++;
        }
        fclose(file);

        //create the random numbers
        srand(getpid() + time(NULL) + getuid());

        char filename[MAXLEN];
        sprintf(filename, "/tmp/%s-%d", getenv("USER"), getpid());
        mkfifo(filename, 0600);
        chmod(filename, 0622);
        printf("Send your requests to %s\n", filename);

        // Continuously loop to handle incoming requests
        while(1) {

                 // Choose a random word from the list
                int randIndex = rand() % numWords;
                
                // Open the FIFO pipe for communication with clients
                FILE *fp = fopen(filename, "r");
                if (!fp) {
                        printf("FIFO %s cannot be opened for reading.\n", filename);
                        exit(2);
                }

                printf("Opened %s to read...\n", filename);

                // Read incoming requests
                while(fgets(line, MAXLEN, fp)) {
                        char *cptr = strchr(line, '\n');
                        if (cptr)
                                *cptr = '\0';
                        // Fork a new process for each client request
                        if(fork() == 0) {
                                // Create a FIFO pipe for communication with the client
                                FILE *clientfp = fopen(line, "w");
                                char serverfifo[MAXLEN];
                                sprintf(serverfifo, "/tmp/%s-%d", getenv("USER"), getpid());
                                mkfifo(serverfifo, 0600);
                                chmod(serverfifo, 0622);

                                 // Send the name of the FIFO pipe for communication with the client
                                fprintf(clientfp, "%s\n", serverfifo);
                                fflush(clientfp);

                                //This will print to let server know what words were selection
                                printf("Selected word: %s\n", words[randIndex]);

                                //Create the hiden words for the client
                                char hiddenWord[MAXLEN];
                                char word[MAXLEN];
                                strcpy(word, words[randIndex]);
                                for(int i = 0; i < strlen(word); i++) {
                                        hiddenWord[i] = '*';
                                 }
                
                                int lengthWord = strlen(word);

                                // Prompt the client for a guess
                                sprintf(clientInput, "(Guess) Enter a letter in the word %s > ", hiddenWord);
                                fprintf(clientfp, "%s\n", clientInput);
                                fflush(clientfp);
                                
                                // Read client guesses and update the hidden word
                                FILE *serverfp = fopen(serverfifo, "r");
                                char request[10];
                                char errorMessage[MAXLEN]; //This will store the error message
                                int missedWord=0;
                                while(lengthWord > 0) {
                                        fscanf(serverfp, "%s", request);
                                        int found = 0;
                                         //This case is the user enter the correct word
                                        for(int i = 0; i < strlen(word); i++) {
                                                if(word[i] == tolower(request[0])) {
                                                        found = 1;
                                                        if(hiddenWord[i] == tolower(request[0])) {
                                                                sprintf(errorMessage, "%6c is already in the word", request[0]);
                                                                fprintf(clientfp, "%s\n", errorMessage);
                                                                 break;
                                                        } else {
                                                                hiddenWord[i] = tolower(request[0]);
                                                                lengthWord--;
                                                        }
                                                }
                                        }

                                        //This case is check if the user enter the wrong word
                                        if(!found) {
                                                missedWord++;
                                                sprintf(errorMessage, "%6c is not in the word", request[0]);
                                                fprintf(clientfp, "%s\n", errorMessage);
                                        }
                                        if(lengthWord > 0) {
                                                sprintf(clientInput, "(Guess) Enter a letter in the word %s > ", hiddenWord);
                                                fprintf(clientfp, "%s\n", clientInput);
                                        }
                                        printf("%s, %s, Guess entered: %c %s\n", serverfifo, line, request[0], hiddenWord);
                                        fflush(clientfp);
                                }

                                // This is the result of the client
                                char result[MAXLEN];
                                sprintf(result, "The word is %s. You missed %d time", words[randIndex], missedWord);
                                fprintf(clientfp, "%s\n", result);
                                fflush(clientfp);

                                unlink(serverfifo);
                                exit(0);
                        }
                }
                fclose(fp);
        }
}





