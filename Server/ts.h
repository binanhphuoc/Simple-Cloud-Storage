#ifndef ts_h
#define ts_h

#include <stdio.h>
#include <stdlib.h>

#define FILE_MAX 1024
#define DIR_MAX 1024
#define ITEMS_SHOWN 8
#define ARG_MAX 20

struct shell{
    char* dirs[DIR_MAX];
    char* files[FILE_MAX];
    char* currentDir;
    int currentDirLength;
    int dirCount;
    int fileCount;
    int dirMarker;
    int fileMarker;
    int sortFlag;
};

int shellService(char* username, int connfd);
time_t getModifiedTime(char *filename, struct shell * info);
void allocateMemory(struct shell * info);            // Allocate memory for global variables
void freeMemory(struct shell * info);                    // Free global variables
int getCurrentDir(struct shell * info);                // Get filename and directories in the current directory
char* printCurrent(struct shell * info);                // Print all fileNames and directories
char getCmd();                            // Get command from user
void processCmdServer(char cmd, int connfd, char* buff, struct shell * info);    // Process command
void processCmdClient(char cmd, int sockfd, char* buff);
int dateCompare(char* s1, char* s2, int ascending, struct shell * info);                    // Compare dates
int nameCompare(char* s1, char* s2, int ascending, struct shell * info);                    // Compare names
void mergeSort(int l, int r, char d,int ascending, struct shell * info);                    // Sort directory

#endif
