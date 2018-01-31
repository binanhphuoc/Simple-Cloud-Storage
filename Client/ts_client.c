/*
	Phuoc Do - 1001239680
	CSE 3320 - 001  Operating system
	Assignment 1 - Introduction to OS Calls, Shell
*/

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <time.h>
#include <curses.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <limits.h>
#include <sys/wait.h>
#include <signal.h>
#include "ts.h"

#define FILE_MAX 1024
#define DIR_MAX 1024
#define ITEMS_SHOWN 8
#define ARG_MAX 20

char* dirs[DIR_MAX];
char* files[FILE_MAX];
char* currentDir = NULL;
int currentDirLength = 256;
int dirCount = 0;
int fileCount = 0;
int dirMarker = 0;
int fileMarker = 0;
int sortFlag = 0;

int getSortFlag()
{
    return sortFlag;
}

// Return 0 if invalid command. Else return command.
char getCmd()
{
	//printf("%s\n", currentDir);
	//getchar();
	char* line = readline("Operation > ");
	if (strlen(line) > 1)
	{
		/*
		system("clear");
		printf("ERROR: Invalid operation. Please try again.\n");
		printf("Press [ENTER] to proceed.\n\n");
		getchar();*/
		return 0;
	}
	else if (strlen(line) == 0)
	{
		return 1;
	}

	return line[0];

}

//return 1 if parse successful
// return 0 if unsucessful
int parsecmd(char* cmd, char** argv)
{
	char* str;
	str = strtok (cmd," ");
	int i = 0;
  while (str != NULL && i < ARG_MAX)
  {
    argv[i] = str;
    str = strtok (NULL, " ");
		i++;
  }

	if (i >= ARG_MAX && str != NULL)
		return 0;
	return 1;
}

int runcmd(char *cmd)
{
  char* argv[ARG_MAX+1] = {NULL};

  pid_t child;
  int child_status;

	parsecmd(cmd,argv);

  child = fork();

  if(child == 0) {
    /* This is done by the child process. */

		execv(argv[0], argv);

		// If execv returns to this point, process is failed.
		system("clear");
		printf("ERROR: Unknown command.\n");
		printf("Press [ENTER] to proceed.\n\n");
		getchar();

    exit(0);
  }
  else {
     /* This is run by the parent.  Wait for the child
        to terminate. */
		 pid_t tpid;
     do {
       tpid = wait(&child_status);
     } while(tpid != child);

     return child_status;
  }
}

void sortCmd(int sockfd, char* buff)
{
	system("clear");
	printf("Operations:\n");
	printf("\tD  Sort by Date\n");
	printf("\tN  Sort by Name\n");
	printf("\tU  Sort by Default\n");
	printf("\n");

    fgets(buff, 3, stdin);
    write(sockfd, buff, 1);
	if (buff[0] == 'D' || buff[0] == 'd')
	{
		printf("Operations:\n");
		printf("\tA  ascending (Oldest to Latest)\n");
		printf("\tD  descending (Latest to Oldest)\n");
		printf("\n");

        fgets(buff, 3, stdin);
        write(sockfd, buff, 1);
		int t = 0;
		if (buff[0] == 'a' || buff[0] == 'A')
			t = 1;
		else if (buff[0] == 'd' || buff[0] == 'D')
			t = 0;
		else if (buff[0] == 1)
			return;
		else
		{
			system("clear");
			printf("ERROR: Invalid operation. Please try again.\n");
			printf("Press [ENTER] to proceed.\n\n");
			fgets(buff, 20, stdin);
            write(sockfd, "k", 1);
			return;
		}
		sortFlag = 1;
		//sortByDate(1);
	}
	else if (buff[0] == 'N' || buff[0] == 'n')
	{
		printf("Operations:\n");
		printf("\tA  ascending (A - Z)\n");
		printf("\tD  descending (Z - A)\n");
		printf("\n");

        fgets(buff, 3, stdin);
        write(sockfd, buff, 1);
		int t = 0;
		if (buff[0] == 'a' || buff[0] == 'A')
			t = 1;
		else if (buff[0] == 'd' || buff[0] == 'D')
			t = 0;
		else if (buff[0] == 1)
			return;
		else
		{
			system("clear");
			printf("ERROR: Invalid operation. Please try again.\n");
			printf("Press [ENTER] to proceed.\n\n");
			fgets(buff, 20, stdin);
            write(sockfd, "k", 1);
			return;
		}

		sortFlag = 1;
		//mergeSort(0, fileCount-1, 'n', t);
		//sortByDate(1);
	}
	else if (buff[0] == 'u' || buff[0] == 'U')
		sortFlag = 0;
	else if (buff[0] == 1)
		return;
	else {
		system("clear");
		printf("ERROR: Invalid operation. Please try again.\n");
		printf("Press [ENTER] to proceed.\n\n");
		fgets(buff, 20, stdin);
        write(sockfd, "k", 1);
	}
}

int retrieveCmd(int sockfd, char* fname)
{
    int bytesReceived = 0;
    char recvBuff[1024];
    memset(recvBuff, '0', sizeof(recvBuff));
    FILE *fp;
    
    //system("clear");
    printf("Retrieve file name: ");
    fgets(fname, 256, stdin);
    fname[strlen(fname)-1]=0;
    write(sockfd, fname, 256);
    
    printf("Receiving file...");
    fp = fopen(fname, "ab");
    if(NULL == fp)
    {
        printf("Error opening file");
        return 1;
    }
    long double sz=0;
    /* Receive data in chunks of 256 bytes */
    while((bytesReceived = read(sockfd, recvBuff, 1024)) > 0)
    {
        //system("clear");
        sz++;
        gotoxy(0,4);
        printf("Received: %d Kb",sz);
        fflush(stdout);
        // recvBuff[n] = 0;
        fwrite(recvBuff, 1,bytesReceived,fp);
        printf("%d \n", bytesReceived);
        if (bytesReceived < 1024)
            break;
        // printf("%s \n", recvBuff);
    }
    
    if(bytesReceived < 0)
    {
        printf("\n Read Error \n");
    }
    printf("\nFile OK....Completed\n");
    fclose(fp);
    fgets(fname,20, stdin);
    write(sockfd, "k", 1);
    return 0;
    
}

int uploadCmd(int sockfd, char* fname)
{
    printf("Retrieve file name: ");
    fgets(fname, 256, stdin);
    fname[strlen(fname)-1]=0;
    write(sockfd, fname, 256);
    
    FILE *fp = fopen(fname,"rb");
    if(fp==NULL)
    {
        printf("File opern error");
        return 1;
    }
    
    /* Read data from file and send it */
    while(1)
    {
        /* First read file in chunks of 256 bytes */
        char buff[1024]={0};
        int nread = fread(buff,1,1024,fp);
        //printf("Bytes read %d \n", nread);
        
        /* If read was success, send data. */
        if(nread > 0)
        {
            //printf("Sending \n");
            write(sockfd, buff, nread);
        }
        if (nread < 1024)
        {
            if (feof(fp))
            {
                printf("End of file\n");
                printf("Done uploading.\n");
            }
            if (ferror(fp))
                printf("Error reading\n");
            break;
        }
    }
    
    fclose(fp);
    fgets(fname,20, stdin);
    write(sockfd, "k", 1);
    return 0;
}

void processCmdClient(char cmd, int sockfd, char* buff)
{
    switch(cmd)
    {
        case 'q':
        case 'Q': exit(0);
        case 'c':
        case 'C':
            printf("Change To > ");
            fgets(buff, 2048, stdin);
            int len = strlen(buff);
            buff[len-1] = 0;
            /*
            system("clear");
            printf("ERROR: Invalid folder. Please try again.\n");
            printf("Press [ENTER] to proceed.\n\n");
            getchar();
             */
            write(sockfd, buff, 2048);
            read(sockfd, buff, 1);
            if (buff[0] == '1')
            {
                system("clear");
                printf("ERROR: Invalid folder. Please try again.\n");
                printf("Press [ENTER] to proceed.\n\n");
                fgets(buff, 20, stdin);
                write(sockfd, "done", 1);
            }
            break;
        case 'n':
        case 'N':
            fileMarker += 8;
            if (fileMarker >= fileCount)
            {
                fileMarker -= 8;
            }
            break;
        case 'p':
        case 'P':
            fileMarker -= 8;
            if (fileMarker < 0)
            {
                fileMarker += 8;
            }
            break;
        case 'd':
        case 'D':
            dirMarker += 8;
            if (dirMarker >= dirCount)
            {
                dirMarker -= 8;
            }
            break;
        case 'a':
        case 'A':
            dirMarker -= 8;
            if (dirMarker < 0)
            {
                dirMarker += 8;
            }
            break;
        case 's':
        case 'S':
            sortCmd(sockfd, buff);
            break;
        case 'r':
        case 'R':
            retrieveCmd(sockfd, buff);
            break;
        case 'u':
        case 'U':
            uploadCmd(sockfd, buff);
            break;
        case 1:
            return;
        default:
            system("clear");
            printf("ERROR: Invalid operation. Please try again.\n");
            printf("Press [ENTER] to proceed.\n\n");
            fgets(buff, 20, stdin);
            write(sockfd, "k", 1);
    }
}

void allocateMemory()
{
	int i;
	for (i = 0; i < FILE_MAX; i++)
	{
		files[i] = (char*) calloc(NAME_MAX+1, sizeof(char));
	}
	for (i = 0; i < DIR_MAX; i++)
	{
		dirs[i] = (char*) calloc(NAME_MAX+1, sizeof(char));
	}
}

void freeMemory()
{
	int i;
	for (i = 0; i < FILE_MAX; i++)
	{
		//printf("In freeMemory %d\n", (files[i] == NULL) ? 1 : 0);
		free(files[i]);
		//printf("In freeMemory\n");
	}
	//printf("In freeMemory\n");
	for (i = 0; i < DIR_MAX; i++)
	{
		free(dirs[i]);
	}
	if (currentDir != NULL)
	{
		free(currentDir);
	}
}

// return 1 if current directory can be reached
// return 0 if number of files and directories exceeds maximum
int getCurrentDir()
{
	DIR * d;
	struct dirent * de;
	time_t t;

	if (currentDir != NULL)
	{
		free(currentDir);
	}

	t = time(NULL);
  //printf("Time: %s\n", ctime(&t));

	currentDirLength = 256;
	currentDir = (char*) calloc(currentDirLength, sizeof(char));
  if (getcwd(currentDir, currentDirLength) == NULL)
	{
		free(currentDir);
		currentDirLength *= 2;
		currentDir = (char*) calloc(currentDirLength, sizeof(char));
	}

  d = opendir(".");
  dirCount = 0;
	fileCount = 0;

	while ((de = readdir(d)))
	{
		if ((de->d_type) & DT_REG)
		{
			if (fileCount > FILE_MAX)
				return 0;
			strcpy(files[fileCount],de->d_name);
			//printf("\t\t%3d. %s\n", fileCount++, de->d_name);
			//getFileCreationTime(s, de->d_name);
			fileCount++;

		}
		else if ((de->d_type) & DT_DIR)
		{
			if (dirCount > DIR_MAX)
				return 0;
			strcpy(dirs[dirCount],de->d_name);
			dirCount++;

		}
	}

	printf("\n");
	closedir(d);
	return 1;
}

void timeString(char* s, time_t t)
{
	strftime(s, 30, "%d %b %Y,%I:%M %p", localtime(&t));
}

char* printCurrent()
{
    char* result = (char*) malloc(sizeof(char) * 2048);
    char line[500];
	system("clear");

	sprintf(line, "Current Working Dir: %s\n", currentDir);
    strcat(result, line);
    
	time_t t;
	t = time(NULL);
	char date[30];
	timeString(date, t);
	sprintf(line,"It is now: %s\n\n", date);
    strcat(result, line);

	int i, count = 0;
	sprintf(line, "Files:\n");
    strcat(result, line);
	for (i = fileMarker; count < ITEMS_SHOWN && i < fileCount;)
	{
		//printf("in printCurrent\n");
		timeString(date, getModifiedTime(files[i]));
		//printf("in printCurrent\n");
		sprintf(line, "\t%d. %-30s%20s\n", i, files[i], date);
        strcat(result, line);
		//printf("\t%d. %-30s\n", i, files[i]);
		i++;
		count++;
	}
	printf("\n");

	count = 0;
	sprintf(line,"Directories:\n");
    strcat(result, line);
	for (i = dirMarker; count < ITEMS_SHOWN && i < dirCount;)
	{
		timeString(date, getModifiedTime(dirs[i]));
		sprintf(line,"\t%d. %-30s%20s\n", i, dirs[i], date);
        strcat(result, line);
		i++;
		count++;
	}
	sprintf(line,"\n");
    strcat(result, line);

	sprintf(line,"Operations:\n");
    strcat(result, line);
	sprintf(line,"\tE  Edit\n");
    strcat(result, line);
	sprintf(line,"\tR  Run\n");
    strcat(result, line);
	sprintf(line,"\tC  Change Directory\n");
    strcat(result, line);
	sprintf(line,"\tS  Sort Directory listing\n");
    strcat(result, line);
	sprintf(line,"\tQ  Quit\n\n");
    strcat(result, line);

	sprintf(line,"Type N: next files\nType P: previous files.\n");
    strcat(result, line);
	sprintf(line,"Type D: next directories\nTyoe A: previous directories.\n\n");
    strcat(result, line);

    return result;
}

time_t getModifiedTime(char *filename)
{
		char name[currentDirLength + NAME_MAX + 2];
		strcpy(name, currentDir);
		strcat(name,"/");
		strcat(name, filename);
    struct stat attrib;
    stat(filename, &attrib);
		return attrib.st_ctime;
}

/////// SORTING

int nameCompare(char* s1, char* s2, int ascending)
{
	if (strcmp(s1, s2) <= 0)
		return ascending;
	else
		return !ascending;
}

int dateCompare(char* s1, char* s2, int ascending)
{
	time_t t1 = getModifiedTime(s1);
	time_t t2 = getModifiedTime(s2);

	if (t1 <= t2)
		return ascending;
	else
		return !ascending;
}

void merge(int left, int mid, int right,int (*compare)(char* s1, char* s2, int ascending), int ascending)
{
    int i, j, k;
    int n1 = mid - left + 1;
    int n2 =  right - mid;

    /* create temp arrays */
    char* Left[n1], *Right[n2];

    /* Copy data to temp arrays L[] and R[] */
    for (i = 0; i < n1; i++)
        Left[i] = files[left + i];
    for (j = 0; j < n2; j++)
        Right[j] = files[mid + 1+ j];

    /* Merge the temp arrays back into arr[l..r]*/
    i = 0; // Initial index of first subarray
    j = 0; // Initial index of second subarray
    k = left; // Initial index of merged subarray
    while (i < n1 && j < n2)
    {
        if (compare(Left[i],Right[j], ascending))
        {
            files[k] = Left[i];
            i++;
        }
        else
        {
            files[k] = Right[j];
            j++;
        }
        k++;
    }

    /* Copy the remaining elements of L[], if there
       are any */
    while (i < n1)
    {
        files[k] = Left[i];
        i++;
        k++;
    }

    /* Copy the remaining elements of R[], if there
       are any */
    while (j < n2)
    {
        files[k] = Right[j];
        j++;
        k++;
    }
}

/* l is for left index and r is right index of the
   sub-array of arr to be sorted */
void mergeSort(int l, int r, char d ,int ascending)
{
    if (l < r)
    {
        // Same as (l+r)/2, but avoids overflow for
        // large l and h
        int m = l+(r-l)/2;

        // Sort first and second halves
        mergeSort(l, m, d, ascending);
        mergeSort(m+1, r, d, ascending);

				if (d == 'd')
        	merge(l, m, r, dateCompare, ascending);
				else
					merge(l, m, r, nameCompare, ascending);
    }
}
