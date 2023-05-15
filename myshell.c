/* CS 347 -- Mini Shell!
 * Original author Phil Nelson 2000
 */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <ctype.h>
#include "argparse.h"
#include "builtin.h"


/* PROTOTYPES */

void processline (char *line);
ssize_t getinput(char** line, size_t* size);
void cancel(void);

char** line;
char** arguments;
int argCount;

/* main
 * This function is the main entry point to the program.  This is essentially
 * the primary read-eval-print loop of the command interpreter.
 */
int main () 
{
 //use getinput and processline as appropriate

  atexit(cancel);

  while(1)
  {
    line = malloc(sizeof(char*));
    *line = NULL;
    size_t size;
    getinput(line, &size);
    processline(*line);
    free(*line);
    free(line);
  }
  return EXIT_SUCCESS;
}


/* getinput
* line     A pointer to a char* that points at a buffer of size *size or NULL.
* size     The size of the buffer *line or 0 if *line is NULL.
* returns  The length of the string stored in *line.
*
* This function prompts the user for insizeof(char *)put, e.g., %myshell%.  If the input fits in the buffer
* pointed to by *line, the input is placed in *line.  However, if there is not
* enough room in *line, *line is freed and a new buffer of adequate space is
* allocated.  The number of bytes allocated is stored in *size. 
* Hint: There is a standard i/o function that can make getinput easier than it sounds.
*/
ssize_t getinput(char** line, size_t* size) 
{
  ssize_t len = 0;
  printf("> ");
  len = getline(line, size, stdin);
  if(len == -1)
  {
    perror("Line Error");
  }
  return len;
}


/* processline
 * The parameter line is interpreted as a command name.  This function creates a
 * new process that executes that command.
 * Note the three cases of the switch: fork failed, fork succeeded and this is
 * the child, fork succeeded and this is the parent (see fork(2)).
 * processline only forks when the line is not empty, and the line is not trying to run a built in command
 */
void processline (char *line)
{
  /*check whether line is empty*/
  if(*line != '\n')
  {
    pid_t cpid;
    int   status;
    argCount;
    arguments = argparse(line, &argCount);
    status = builtIn(arguments, argCount);

    /*check whether arguments are builtin commands
    *if not builtin, fork to execute the command.
    */
    if(status == 0)
    {
      cpid = fork();
      if(cpid == 0)
      {
        int result = execvp(arguments[0], arguments);
        if(result < 0)
        {
          fprintf(stderr, "%s: command not found\n", arguments[0]);
        }
      }
      else if(cpid > 0)
      {
        pid_t p = wait(&status);
      }
      else{
        perror("ERROR");
      }
    }

    //Free up all memory in arguments
    for(int i = 0; i < argCount; i++)
    {
      free(arguments[i]);
    }
    free(arguments);
  }
}

/*
This function is to free all the memory in line and arguments.
*/
void cancel(void)
{
  free(*line);
  free(line);
  for(int i = 0; i < argCount; i++)
  {
    free(arguments[i]);
  }
  free(arguments);
}
