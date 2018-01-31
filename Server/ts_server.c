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


int shellService(char* username, int connfd)
{
    struct shell info;
    
    info.currentDir = NULL;
    info.currentDirLength = 256;
    info.dirCount = 0;
    info.fileCount = 0;
    info.dirMarker = 0;
    info.fileMarker = 0;
    info.sortFlag = 0;
    
    char buff[2048];
    char homeFolder[80];
    strcpy(homeFolder, "/home/");
    strcat(homeFolder, username);
		if (chdir(homeFolder) == -1)
		{
			system("clear");
			printf("ERROR: Specified directory does not exist. Program will run on current directory.\n");
			printf("Press [ENTER] to proceed.\n\n");
			//getchar();
		}
	allocateMemory(&info);
	while (1)
	{
		if (!info.sortFlag)
		{
			if (info.currentDir != NULL)
				free(info.currentDir);
			info.currentDir = NULL;
            getCurrentDir(&info);
		}
        
        char* dirInfo = printCurrent(&info);
        int err = write(connfd, dirInfo, 2048);
        if (err == -1)
        {
            printf("Error in write\n");
            return 1;
        }
        free(dirInfo);
        
        read(connfd, buff, 1);
        char k = buff[0];
        if (k == 1)
            continue;
        if (k == 'q' || k == 'Q')
            break;
        processCmdServer(k, connfd, buff, &info);
        
	}
	freeMemory(&info);
	return 0;
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

void sortCmd(int connfd, char* buff, struct shell * info)
{
    read(connfd, buff, 1);
	if (buff[0] == 'D' || buff[0] == 'd')
	{
		read(connfd, buff, 1);
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
			getchar();
			return;
		}
		info->sortFlag = 1;
		mergeSort(0, info->fileCount-1, 'd', t, info);
		//sortByDate(1);
	}
	else if (buff[0] == 'N' || buff[0] == 'n')
	{
		read(connfd, buff, 1);
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
			getchar();
			return;
		}

		info->sortFlag = 1;
		mergeSort(0, info->fileCount-1, 'n', t, info);
		//sortByDate(1);
	}
	else if (buff[0] == 'u' || buff[0] == 'U')
		info->sortFlag = 0;
	else if (buff[0] == 1)
		return;
	else {
		read(connfd, buff, 1);
	}
}

int retrieveCmd(int connfd, char* fname)
{
    read(connfd, fname, 256);
    printf("%s \n", fname);
    
    strcat(fname, ".enc");
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
            int i;
            //printf("Sending \n");
            for (i = 0; i < nread; i++)
            {
                buff[i] -= 22;
            }
            write(connfd, buff, nread);
        }
        if (nread < 1024)
        {
            if (feof(fp))
            {
                printf("End of file\n");
                printf("File transfer completed for id: %d\n",connfd);
            }
            if (ferror(fp))
                printf("Error reading\n");
            break;
        }
    }
    
    fclose(fp);
    read(connfd, fname, 1);
    return 0;
    
}

void gotoxy(int x,int y)
{
    printf("%c[%d;%df",0x1B,y,x);
}

int uploadCmd(int connfd, char* fname)
{
    read(connfd, fname, 256);
    
    int bytesReceived = 0;
    char recvBuff[1024];
    memset(recvBuff, '0', sizeof(recvBuff));
    FILE *fp;
    
    printf("Receiving file...");
    strcat(fname, ".enc");
    fp = fopen(fname, "ab");
    if(NULL == fp)
    {
        printf("Error opening file");
        return 1;
    }
    long double sz=0;
    /* Receive data in chunks of 256 bytes */
    while((bytesReceived = read(connfd, recvBuff, 1024)) > 0)
    {
        //system("clear");
        sz++;
        gotoxy(0,4);
        printf("Received: %d Kb",sz);
        fflush(stdout);
        // recvBuff[n] = 0;
        int i;
        for (i = 0; i < bytesReceived; i++)
        {
            recvBuff[i] += 22;
        }
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
    read(connfd, fname, 1);
    return 0;
}

void processCmdServer(char cmd, int connfd, char* buff, struct shell * info)
{
    printf("%c\n",cmd);
	switch(cmd)
	{
		case 'c':
		case 'C':
            read(connfd, buff, 2048);
            printf("Folder name: %s", buff);
			if (chdir(buff) == -1)
			{
                strcpy(buff, "1"); // error
                write(connfd, buff, 1);
                read(connfd, NULL, 1);
			}
            else
            {
                strcpy(buff, "0"); // no error
                write(connfd, buff, 1);
            }
            
			break;
		case 'n':
		case 'N':
			info->fileMarker += 8;
			if (info->fileMarker >= info->fileCount)
			{
				info->fileMarker -= 8;
			}
			break;
		case 'p':
		case 'P':
			info->fileMarker -= 8;
			if (info->fileMarker < 0)
			{
				info->fileMarker += 8;
			}
			break;
		case 'd':
		case 'D':
			info->dirMarker += 8;
			if (info->dirMarker >= info->dirCount)
			{
				info->dirMarker -= 8;
			}
			break;
		case 'a':
		case 'A':
			info->dirMarker -= 8;
			if (info->dirMarker < 0)
			{
				info->dirMarker += 8;
			}
			break;
		case 's':
		case 'S':
			sortCmd(connfd, buff, info);
			break;
        case 'r':
        case 'R':
            retrieveCmd(connfd, buff);
            printf("Debug\n");
            return;
        case 'u':
        case 'U':
            uploadCmd(connfd, buff);
            break;
		case 1:
			return;
		default:
			system("clear");
			printf("ERROR: Invalid operation. Please try again.\n");
			printf("Press [ENTER] to proceed.\n\n");
            read(connfd, buff, 1);
	}
}

void allocateMemory(struct shell * info)
{
	int i;
	for (i = 0; i < FILE_MAX; i++)
	{
		info->files[i] = (char*) calloc(NAME_MAX+1, sizeof(char));
	}
	for (i = 0; i < DIR_MAX; i++)
	{
		info->dirs[i] = (char*) calloc(NAME_MAX+1, sizeof(char));
	}
}

void freeMemory(struct shell * info)
{
	int i;
	for (i = 0; i < FILE_MAX; i++)
	{
		//printf("In freeMemory %d\n", (files[i] == NULL) ? 1 : 0);
		free(info->files[i]);
		//printf("In freeMemory\n");
	}
	//printf("In freeMemory\n");
	for (i = 0; i < DIR_MAX; i++)
	{
		free(info->dirs[i]);
	}
	if (info->currentDir != NULL)
	{
		free(info->currentDir);
        info->currentDir = NULL;
	}
}

// return 1 if current directory can be reached
// return 0 if number of files and directories exceeds maximum
int getCurrentDir(struct shell * info)
{
	DIR * d;
	struct dirent * de;
	time_t t;

	if (info->currentDir != NULL)
	{
		free(info->currentDir);
	}
    info->currentDir = NULL;

	t = time(NULL);
  //printf("Time: %s\n", ctime(&t));

	info->currentDirLength = 256;
	info->currentDir = (char*) calloc(info->currentDirLength, sizeof(char));
  if (getcwd(info->currentDir, info->currentDirLength) == NULL)
	{
		free(info->currentDir);
		info->currentDirLength *= 2;
		info->currentDir = (char*) calloc(info->currentDirLength, sizeof(char));
	}

  d = opendir(".");
  info->dirCount = 0;
	info->fileCount = 0;

	while ((de = readdir(d)))
	{
		if ((de->d_type) & DT_REG)
		{
			if (info->fileCount > FILE_MAX)
				return 0;
			strcpy(info->files[info->fileCount],de->d_name);
			//printf("\t\t%3d. %s\n", fileCount++, de->d_name);
			//getFileCreationTime(s, de->d_name);
			info->fileCount++;

		}
		else if ((de->d_type) & DT_DIR)
		{
			if (info->dirCount > DIR_MAX)
				return 0;
			strcpy(info->dirs[info->dirCount],de->d_name);
			info->dirCount++;

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

char* printCurrent(struct shell * info)
{
    char* result = (char*) malloc(sizeof(char) * 2048);
    char line[500];
	system("clear");

	sprintf(line, "Current Working Dir: %s\n", info->currentDir);
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
	for (i = info->fileMarker; count < ITEMS_SHOWN && i < info->fileCount;)
	{
		//printf("in printCurrent\n");
		timeString(date, getModifiedTime(info->files[i], info));
		//printf("in printCurrent\n");
		sprintf(line, "\t%d. %-30s%20s\n", i, info->files[i], date);
        strcat(result, line);
		//printf("\t%d. %-30s\n", i, files[i]);
		i++;
		count++;
	}
    sprintf(line,"\n");
    strcat(result, line);
    
	count = 0;
	sprintf(line,"Directories:\n");
    strcat(result, line);
	for (i = info->dirMarker; count < ITEMS_SHOWN && i < info->dirCount;)
	{
		timeString(date, getModifiedTime(info->dirs[i], info));
		sprintf(line,"\t%d. %-30s%20s\n", i, info->dirs[i], date);
        strcat(result, line);
		i++;
		count++;
	}
	sprintf(line,"\n");
    strcat(result, line);

	sprintf(line,"Operations:\n");
    strcat(result, line);
	sprintf(line,"\tC  Change Directory\n");
    strcat(result, line);
	sprintf(line,"\tS  Sort Directory listing\n");
    strcat(result, line);
    sprintf(line,"\tR  Retrieve file\n");
    strcat(result, line);
    sprintf(line,"\tU  Upload file\n");
    strcat(result, line);
	sprintf(line,"\tQ  Quit\n\n");
    strcat(result, line);

	sprintf(line,"Type N: next files\nType P: previous files.\n");
    strcat(result, line);
	sprintf(line,"Type D: next directories\nTyoe A: previous directories.\n\n");
    strcat(result, line);

    return result;
}

time_t getModifiedTime(char *filename, struct shell * info)
{
		char name[info->currentDirLength + NAME_MAX + 2];
		strcpy(name, info->currentDir);
		strcat(name,"/");
		strcat(name, filename);
    struct stat attrib;
    stat(filename, &attrib);
		return attrib.st_ctime;
}

/////// SORTING

int nameCompare(char* s1, char* s2, int ascending, struct shell * info)
{
	if (strcmp(s1, s2) <= 0)
		return ascending;
	else
		return !ascending;
}

int dateCompare(char* s1, char* s2, int ascending, struct shell * info)
{
	time_t t1 = getModifiedTime(s1, info);
	time_t t2 = getModifiedTime(s2, info);

	if (t1 <= t2)
		return ascending;
	else
		return !ascending;
}

void merge(int left, int mid, int right,int (*compare)(char* s1, char* s2, int ascending, struct shell * info), int ascending, struct shell * info)
{
    int i, j, k;
    int n1 = mid - left + 1;
    int n2 =  right - mid;

    /* create temp arrays */
    char* Left[n1], *Right[n2];

    /* Copy data to temp arrays L[] and R[] */
    for (i = 0; i < n1; i++)
        Left[i] = info->files[left + i];
    for (j = 0; j < n2; j++)
        Right[j] = info->files[mid + 1+ j];

    /* Merge the temp arrays back into arr[l..r]*/
    i = 0; // Initial index of first subarray
    j = 0; // Initial index of second subarray
    k = left; // Initial index of merged subarray
    while (i < n1 && j < n2)
    {
        if (compare(Left[i],Right[j], ascending, info))
        {
            info->files[k] = Left[i];
            i++;
        }
        else
        {
            info->files[k] = Right[j];
            j++;
        }
        k++;
    }

    /* Copy the remaining elements of L[], if there
       are any */
    while (i < n1)
    {
        info->files[k] = Left[i];
        i++;
        k++;
    }

    /* Copy the remaining elements of R[], if there
       are any */
    while (j < n2)
    {
        info->files[k] = Right[j];
        j++;
        k++;
    }
}

/* l is for left index and r is right index of the
   sub-array of arr to be sorted */
void mergeSort(int l, int r, char d ,int ascending, struct shell * info)
{
    if (l < r)
    {
        // Same as (l+r)/2, but avoids overflow for
        // large l and h
        int m = l+(r-l)/2;

        // Sort first and second halves
        mergeSort(l, m, d, ascending, info);
        mergeSort(m+1, r, d, ascending, info);

				if (d == 'd')
        	merge(l, m, r, dateCompare, ascending, info);
				else
					merge(l, m, r, nameCompare, ascending, info);
    }
}
