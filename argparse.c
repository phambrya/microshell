#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "argparse.h"
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

#define FALSE (0)
#define TRUE  (1)

/*
* argCount is a helper function that takes in a String and returns the number of "words" in the string assuming that whitespace is the only possible delimeter.
*/
static int argCount(char*line)
{
  int count = 0;
  int word = FALSE;
  while(*line != '\0')
  {
    if(isspace(*line))
    {
      if(word == 1)
      {
        word = FALSE;
        count++;
      }
    }
    else
    {
      word = TRUE;
    }
    line++;
  }
  if(word == 1)
  {
    count++;
  }
  return count;
}


/*
*
* Argparse takes in a String and returns an array of strings from the input.
* The arguments in the String are broken up by whitespaces in the string.
* The count of how many arguments there are is saved in the argcp pointer
*/
char** argparse(char* line, int* argcp)
{
  *argcp = argCount(line);

  char **ptr = malloc((*argcp +1) * sizeof(char*));

  for(int i = 0; i < *argcp; i++)
  {
    char *word = malloc(strlen(line) + 1);
    char *ret = word;
    //Taking care of white spaces
    while(isspace(*line))
    {
      line++;
    }
    //Add to word if it is not a white space
    while(!isspace(*line))
    {
      *word++ = *line++;
    }
    *word = '\0';
    char *smaller_str = malloc(strlen(ret) + 1);
    strcpy(smaller_str, ret);
    ptr[i] = smaller_str;
    free(ret);
  }
  ptr[*argcp] = NULL;
  return ptr;
}


