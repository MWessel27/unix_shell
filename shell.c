#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

/*
Name: executeCmd
Description: Execute a command on a forked process and wait for the result
Parameters:
  - args : command name and arguments to pass to the command
Returns: A status of whether or not we are done executing the command in the
          background process
*/
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

  //fork a separate process
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

/*
Name: getArgs
Description: Gets the arguments from a specified CLI
Parameters:
  - cmd : A CLI call to execute along with parameters
Returns: The formatted arguments with the command line function to pair them with
*/
//define delimeters we will use for new line, tabs, and spaces
#define TOKEN_DELIMETERS "\n\t "
char **getArgs(char *cmd)
{
  //location for the array we will construct of arguments
  int location = 0;
  //start with a size that will act like a buffer
  int argSize = 64;
  //initialize/declare token and token delimeters of a size
  char *token;
  //allocate memory for the token delimeters
  char **token_delims = malloc(argSize);

  //call strtok with the cmd and delimeters to get our tokens
  token = strtok(cmd, TOKEN_DELIMETERS);
  while (token != NULL) {
    //add each token
    token_delims[location] = token;
    location++;
    //if we go over the memory, allocate more
    if (location >= argSize) {
      argSize += argSize;
      token_delims = realloc(token_delims, argSize);
    }
    //go until we hit null
    token = strtok(NULL, TOKEN_DELIMETERS);
  }
  //end the delimeters with null
  token_delims[location] = NULL;
  return token_delims;
}

/*
Name: readPrompt
Description: Waits for user to enter commands and then reads them from stdin
Parameters: None
Returns: The command that the user wants to run with the arguments they want to pass
*/
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

/*
Name: strdup
Description: Do a string duplication of memory location for the passed in string
Parameters:
  - str : String to be duplicated
Returns: Dupilicated string memory location
*/
char *strdup(const char *str)
{
  char *cpy = NULL;
  if (str)
  {
  cpy = malloc(strlen(str)+1);
  if (cpy)
  strcpy(cpy, str);
  }
  return cpy;
}

/*
Name: split
Description: Split a string by a delimeter
Parameters:
  - str : String to be split
  - delimeter : Delimeter to split string
Returns: The split string
*/
char **split(char *str, char delimeter)
{
    char **result = 0;
    //create a delimeter object
    char delim[2];
    delim[0] = delimeter;
    delim[1] = 0;

    //allocate memory for the result
    result = malloc(512);

    //create a size holder and token object
    size_t idx  = 0;
    char* token = strtok(str, delim);

    while (token)
    {
        //while we have a token, duplicate the split string and point the result
        // to the memory location
        *(result + idx++) = strdup(token);
        token = strtok(0, delim);
    }
    *(result + idx) = 0;

    return result;
}

/*
Name: executeBatch
Description: Execute a batch file that is passed in
Parameters:
  - args : stdin
Returns: An integer state to let us know to continue running or to stop
*/
int executeBatch(char *args) {
  //first lets initialize variables to read from a file
  FILE *fp;
  char str[60];
  char **cmdArgs;
  char **splitCmd;
  //open the file for read operations
  fp = fopen(args , "r");
  if(fp == NULL)
  {
     //fail if we can't open the file
     perror("Error opening file");
     return EXIT_FAILURE;
  }
  while( fgets (str, 60, fp)!=NULL )
  {
      //read the file in one line at a time
      printf("prompt>%s", str);
      //lets split the command by semi colon
      splitCmd = split(str, ';');

      //get the parsed string object from the split function
      for (int i = 0; *(splitCmd + i); i++)
      {
          //get the arguments from the command
          cmdArgs = getArgs(*(splitCmd + i));
          //execute the command and return whether or not we are still running
          executeCmd(cmdArgs);
      }
      printf("\n");
  }
  fclose(fp);
  return EXIT_SUCCESS;
}

/*
Name: prompt
Description: Continuously prompt the user to enter commands into stdin
Parameters: None
Returns: Void
*/
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

/*
Name: main
Description: Main function
*/
int main(int argc, char *argv[])
{
  if(argc > 1) {
    // call to execute a batch file of commands
    executeBatch(argv[1]);
  } else {
    // call function to begin the prompt
    prompt();
  }
  return EXIT_SUCCESS;
}
