# unix_shell

# Description
Creating a Unix Shell program similar to Terminal. Basic functionality includes
the ability to run any shell command as well as a series of commands and arguments.

Examples:
1. Single commands
  prompt> ls
  prompt> pwd
2. Commands with arguments
  prompt> ls -la
  prompt> ifconfig -a
3. Commands separated by a semicolon
  prompt> ls ; pwd
  prompt> ls -la; ifconfig -a; pwd

4. Batch file support
  ./tinysh batch_file.txt

# Create Executable tinysh
gcc -std=c99 -Wall -Wextra -Werror -o tinysh shell.c

# Exit
Exit the program by typing 'quit' or hitting Ctrl-D on the keyboard

# Author
Created by Mikal Wessel for class COP4610.
FSU ID: mdw15d
