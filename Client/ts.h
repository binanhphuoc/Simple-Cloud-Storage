#ifndef ts_h
#define ts_h

#include <stdio.h>
#include <stdlib.h>

int shellService(char* username, int connfd);
time_t getModifiedTime(char *filename);
void allocateMemory();            // Allocate memory for global variables
void freeMemory();                    // Free global variables
int getCurrentDir();                // Get filename and directories in the current directory
char* printCurrent();                // Print all fileNames and directories
char getCmd();                            // Get command from user
void processCmdServer(char cmd, int connfd, char* buff);    // Process command
void processCmdClient(char cmd, int sockfd, char* buff);
int dateCompare(char* s1, char* s2, int ascending);                    // Compare dates
int nameCompare(char* s1, char* s2, int ascending);                    // Compare names
void mergeSort(int l, int r, char d,int ascending);                    // Sort directory

#endif
