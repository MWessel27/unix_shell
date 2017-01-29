#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int executeCmd(char **args){
  //this is where we will double check that the command is not empty
  // or if we need to exit the terminal
  if(args[0]==NULL){
    return 1;
  }
  if(strcmp(args[0],"quit") == 0){
    return 0;
  }

  //declare pid int and status int
  int pid;
  int status;

  pid = fork();

  if(pid == 0) {
    if(execvp(args[0], args) == -1){
      //if the argument is not valid, let the user know
      printf("Not a valid command!\n");
      return 1;
    }
  } else {
    do {
    //wait for the pid to return
    waitpid(pid, &status, 0);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
  }

  return 1;
}

#define TOKEN_DELIMETERS "\n\t "
char **getArgs(char *cmd)
{
  //location for the array we will construct of arguments
  int location = 0;
  //start with a size that will act like a buffer
  int argSize = 64;
  //initialize/declare token and token delimeters of a size
  char *token;
  char **token_delims = malloc(argSize);

  token = strtok(cmd, TOKEN_DELIMETERS);
  while (token != NULL) {
    token_delims[location] = token;
    location++;

    if (location >= argSize) {
      argSize += argSize;
      token_delims = realloc(token_delims, argSize);
    }

    token = strtok(NULL, TOKEN_DELIMETERS);
  }
  token_delims[location] = NULL;
  return token_delims;
}

int executeBatch(char *args) {
  FILE *fp;
  char str[60];
  char **cmdArgs;

  /* opening file for reading */
  fp = fopen(args , "r");
  if(fp == NULL)
  {
     perror("Error opening file");
     return EXIT_FAILURE;
  }
  while( fgets (str, 60, fp)!=NULL )
  {
     printf("prompt>%s", str);
     //get the arguments from the command
     cmdArgs = getArgs(str);
     //execute the command and return whether or not we are still running
     executeCmd(cmdArgs);
  }
  fclose(fp);
  return EXIT_SUCCESS;
}

char *readPrompt(){
  //since the input could be over 512 char, we need to allocate
  // 512 command length once. If it is more than 512, we will allocate more
  int cmdLength = 512;
  char *cmd = malloc(cmdLength);

  //in C, EOF is an int, so if I want to check for that charLocation is an int
  int charLocation;
  //this will be the size and location of where to put the character
  int size = 0;

  while (1) {
    //get the first character as an int
    charLocation = getchar();

    //since the char as an int could be an EOF int, or it could be a new line
    // check for it here
    if (charLocation == '\n' || charLocation == EOF || charLocation == ';') {
      //to avoid a null pointer, add a null character
      cmd[size] = '\0';
      return cmd;
    } else {
      //otherwise, add the character to the command
      cmd[size] = charLocation;
    }
    //increment the size of the command
    size++;

    //constants on the left of the comparison
    // if the command is longer than 512 characters, add more
    if (size >= cmdLength) {
      cmdLength += 512;
      //reallocate the increased length to the command
      cmd = realloc(cmd, cmdLength);
    }
  }
}

void prompt()
{
  int running=1;
  char *cmd;
  char **cmdArgs;

  do {
    //first print out the prompt
    printf("prompt> ");
    //next read the command from the prompt
    cmd = readPrompt();
    //get the arguments from the command
    cmdArgs = getArgs(cmd);
    //execute the command and return whether or not we are still running
    running = executeCmd(cmdArgs);
  } while (running);
}


int main(int argc, char *argv[])
{
  if(argc > 1) {
    executeBatch(argv[1]);
  } else {
    // call function to begin the prompt
    prompt();
  }
  return EXIT_SUCCESS;
}
