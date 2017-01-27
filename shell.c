#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int executeCmd(char *args){
  //this is where we will double check that the command is not empty
  // or if we need to exit the terminal
  if(strcmp(args,"quit") == 0){
    return 0;
  }

  int pid;
  int status;

  pid = fork();
  
  char *cmd = args;
  char *argv[3];
  argv[0] = args;
  argv[1] = NULL;
  //argv[2] = NULL;

  if(pid == 0) {
    if(execvp(cmd, argv) == -1){
      printf("Not a valid command!\n");
      return 1;
    }
  } else {
    do {
    waitpid(pid, &status, 0);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
  }

  return 1;
}

void executeBatch(char *args) {
  FILE *fp;
  char str[60];

  /* opening file for reading */
  fp = fopen(args , "r");
  if(fp == NULL)
  {
     perror("Error opening file");
     //return(-1);
  }
  if( fgets (str, 60, fp)!=NULL )
  {
     puts(str);
  }
  fclose(fp);
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
    if (charLocation == ';' || charLocation == '\n' || charLocation == EOF) {
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

  do {
    //first print out the prompt
    printf("prompt> ");
    //next read the command from the prompt
    cmd = readPrompt();
    //execute the command and return whether or not we are still running
    running = executeCmd(cmd);
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
