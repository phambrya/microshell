/*
Bryan Pham
Project 2: Microshell
Due: May 10, 2022
Description: Create a microshell, myshell, that adheres to 
the following requirements. The program will be written in C. 
myshell will parse the user input. myshell has its own set of 
built-in commands.
*/

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h> 
#include <fcntl.h> 
#include <ctype.h>
#include "builtin.h"
#include <string.h>
#include <dirent.h>
#include <pwd.h>
#include <time.h>
#include <grp.h>

//Prototypes
static void exitProgram(char** args, int argcp);
static void cd(char** args, int argcp);
static void pwd(char** args, int argcp);

static void statData(char** args, int argcp);
static void Timeprint(struct stat s, time_t t);
static void tailOpenPrintFile(char* path, int lines, int argcp);
static void tailProgram(char** args, int argcp);
static void envProgram(char** args, int argcp);

/* builtIn
 ** built in checks each built in command the given command, if the given command
 * matches one of the built in commands, that command is called and builtin returns 1.
 *If none of the built in commands match the wanted command, builtin returns 0;
  */
int builtIn(char** args, int argcp)
{
  if(argcp < 1)
  {
    return 0;
  }
  if(strcmp(args[0], "exit") == 0) 
  {
    exitProgram(args, argcp);
    return 1;
  }
  else if(strcmp(args[0], "pwd") == 0) 
  {
    pwd(args, argcp);
    return 1;
  }
  else if(strcmp(args[0], "cd") == 0)
  {
    cd(args, argcp);
    return 1;
  }
  else if(strcmp(args[0], "stat") == 0)
  {
    statData(args, argcp);
    return 1;
  }
  else if(strcmp(args[0], "tail") == 0)
  {
    tailProgram(args, argcp);
    return 1;
  }
  else if(strcmp(args[0], "env") == 0)
  {
    envProgram(args, argcp);
    return 1;
  }
  else
  {
    return 0;
  }
}

/*
This function is to exit.
exit[value]: exit the shell with the value.
If value is not given, exit with value 0.
*/
static void exitProgram(char** args, int argcp)
{
  if(argcp > 1)
  {
    exit(atoi(args[1]));
  }
  else
  {
    exit(0);
  }
}

/*
This function call pwd so it will print the current working directory.
*/
static void pwd(char** args, int argcp)
{
  if(argcp == 1)
  {
    size_t size = pathconf("/", _PC_PATH_MAX);
    char path[size];
    getcwd(path, size);
    printf("%s\n", path);
  }
  else
  {
    fprintf(stderr, "bash: pwd: too many arguments\n");
  }
}

/*
cd[directory]: It will change the current working directory.
When no parameters are used, cd will change to the home directory.
One dot refers to current directory, and dot dot refers to the parents directory.
*/
static void cd(char** args, int argcp)
{
  if(argcp > 2)
  {
    fprintf(stderr, "bash: cd: too many arguments\n");
  }
  if(argcp == 2)
  {
    if(chdir(args[1]) == -1) 
    {
      fprintf(stderr, "bash: cd: %s: %s\n",args[1], strerror(errno));
    }
  }
  else
  {
    chdir(getenv("HOME"));
  }
}

/*
stat <file1 or directory1...fileN or directoryN>: prints the information 
about the files. The information includes file name, user name, group name,
permissions, number of links, size, and modification time.
*/
static void statData(char** args, int argcp)
{
  if(argcp > 1)
  {
    struct stat stats;
    for(int i = 1; i < argcp; i++)
    {        
      if(stat(args[i], &stats) == -1)
      {
        fprintf(stderr, "stat: cannot stat '%s': %s\n", args[i], strerror(errno));
      }
      else
      {
        //Getting username id
        printf("  File: %s\n", args[i]);
        struct passwd *pas;
        pas = getpwuid(stats.st_uid);

        //Getting group id
        struct group *group;
        group = getgrgid(stats.st_gid);

        //Print all the information for stat.
        printf("  Size: %li              Blocks: %li          IO Block: %li\n", stats.st_size, stats.st_blocks, stats.st_blksize);
        printf("Device: %lXh/%ldd         Inode: %lu       Links: %lu\n", stats.st_dev, stats.st_dev, stats.st_ino, stats.st_nlink);

        printf("Access: (/");
        printf("%04o", stats.st_mode & 077777);
        printf((S_ISDIR(stats.st_mode)) ? "d" : "-");
        printf((stats.st_mode & S_IRUSR) ? "r" : "-");
        printf((stats.st_mode & S_IWUSR) ? "w" : "-");
        printf((stats.st_mode & S_IXUSR) ? "x" : "-");
        printf((stats.st_mode & S_IRGRP) ? "r" : "-");
        printf((stats.st_mode & S_IWGRP) ? "w" : "-");
        printf((stats.st_mode & S_IXGRP) ? "x" : "-");
        printf((stats.st_mode & S_IROTH) ? "r" : "-");
        printf((stats.st_mode & S_IWOTH) ? "w" : "-");
        printf((stats.st_mode & S_IXOTH) ? "x)" : "-)  ");

        printf("Uid: (%u/  %s)   Gid: (%u/%s)\n", stats.st_uid, pas->pw_name, stats.st_gid, group->gr_name);

        //Using an helper function to print out the right time.
        printf("Access: ");
        Timeprint(stats, stats.st_atime);
        printf("Modify: ");
        Timeprint(stats, stats.st_mtime);
        printf("Change: ");
        Timeprint(stats, stats.st_ctime);
      }
    }
  }
  else
  {
    fprintf(stderr, "stat: missing operand\n");
  }
}

/*
This method is a helper function for statData to print all 
the time for Access, Modify, and Change.
*/
static void Timeprint(struct stat s, time_t t)
{
  struct tm *tm;
  tm = localtime(&t);
  printf("%i-%02i-%02i", tm->tm_yday + 1900, tm->tm_mon +1, tm->tm_mday);
  printf(" %02i:%02i:%02i\n", tm->tm_hour, tm->tm_min, tm->tm_sec);
}

/*
This is a helper function for tailProgram where this will open and print the last 10 line of the provide file.
*/
static void tailOpenPrintFile(char* path, int lines, int argcp)
{
  //This open the file and check if the file exist
  FILE* file = fopen(path, "r");

  if (file == NULL) {
    fprintf(stderr, "tail: %s: %s\n", path, strerror(errno));
    return;
  }

  if(argcp > 2)
  {
    printf("==> %s <==\n", path);
  }

  //Go to the end of the file and go backward 10 lines
  fseek(file, 0, SEEK_END);
  for (int pos = ftell(file); pos >= 0; pos--) {
    if (fgetc(file) == '\n') {
      if (--lines == 0) {
        break;
      }
    }
    fseek(file, -2, SEEK_CUR);
  }

  char *line = NULL;
  size_t len = 0;
  ssize_t read;
  //This prints the last 10 line of the file
  while ((read = getline(&line, &len, file)) != -1) {
    printf("%s", line);
  }
  printf("\n");
  free(line);

  fclose(file);
}

/*
This function the main function for tell where it will call tailOpenPrintFile.
There is a forloop where it will prove the helper function with the argument.
*/
static void tailProgram(char** args, int argcp)
{
  size_t numOfLine = 10;
  if(argcp == 1)
  {
    fprintf(stderr, "not enough arguments\n");
  }
  else 
  {
    for (int i = 1; i < argcp; i++) 
    {
      tailOpenPrintFile(args[i], numOfLine, argcp);
    }
  }
}

/*
env [NAME=VALUE]: prints the environment variables list. When Name=Value is used, 
it sets the environment variable of the same name to the given value.
*/
extern char **environ;
static void envProgram(char** args, int argcp)
{
  int invalid = 0;
  for(int i = 1; i < argcp; i++)
  {  
    size_t size;
    char *name;
    char *value;
    const char *arg = args[i];
    const char *equal = "=";
    size_t counter = strcspn(arg, equal);
    size_t rest = strlen(arg) - (counter + 1);

    //Check if the environment can be process with name and value
    if(counter >= strlen(arg) -1 || counter == 0)
    {
      fprintf(stderr, "env: '%s': No such file or directory\n", arg);
      invalid = 1;
      break;
    }

    name = malloc(counter+1);
    value = malloc(rest+1);

    strncpy(name, arg, counter);
    name[counter] = '\0';
    arg += counter + 1;
    strncpy(value, arg, rest);
    value[rest] = '\0'; 

    setenv(name, value, 1);
    free(name);
    free(value);
  }
  
  //Print the environment information
  char **env = environ;

  while(*env != NULL && !invalid)
  {
    printf("%s\n", *env);
    env++;
  }
}
