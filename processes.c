/*----------------------------------------------------------------------------*
 *  CShell v1.0                                                               *
 *  Fileame:      cshell.c                                                    *
 *  Written by:   Corby Berry - 0665977                                       *
 *                                                                            *
 *  Purpose:      This program acts as a little 'shell'. Users can run        *
 *                this program in the command line and accepts a select       *
 *                few verb names as arguments. The shell interprets these     *
 *                commands and executes associated bash commands or macros.   *
 *                                                                            *
 *  Usage         ./cshell [verb]                                             *
 *  Subrtns/Libs: See includes.                                               *
/*----------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>

int main(int argc, char* argv[]) {
  /* Function Prototypes */
  char* readline();           // Reads input from the user.
  void launch();              // Launches a given command/program.

  /* Variables */
  char* input;                // The input string or verb the user enters.
  char* command;              // The actual command that gets executed.

  // The main program loop that runs as long as the user does not input "exit".
  do
  {
    // Get input from the user.
    input = readline();
    
    // So because this is a post-check loop, it still printed "usage" when I typed exit.
    // Now though, I get a segmentation fault when I try to turn it to a pre-check loop.
    // Hence this dumb "if" condition that should be sentinel for the loop... >:(
    if(strcmp(input, "exit")==0)
    {
      continue;
    }
    // If-else ladder that parses the input string to check for commands:
    if(strcmp(input, "help")==0)
    {
      // Display some help text. (I don't know if I should fork this.
      // It isn't a standard UNIX process so, methinks not).
      printf("help:\t\t\tdisplays this text.\n");
      printf("filestats:\t\tlists file permissions, links, owner name and group,\n\t\t\t\t");
      printf("filesize, modtime, and name in current dir.\n");
      printf("diskuse:\t\tshows the amount of disk space being used in the\n\t\t\t\t");
      printf("current directory\ndrives:\t\t\tlists all of the ext4 mounted partitions as well\n");
      printf("\t\t\t\tas their sizes.\nexit:\t\t\texits the shell.\n");
    }
    else if(strcmp(input, "filestats")==0)
    {
      command = "/bin/ls";
      launch(command);
    }
    else if(strcmp(input, "diskuse")==0)
    {
      command = "/bin/du";
      launch(command);
    }
    else if(strcmp(input, "drives")==0)
    {
      command = "/bin/df";
      launch(command);
    }
    else
    {
      printf("cshell: %s: command not found.\n", input);
      printf("Try 'help' for some help.\n");
    }
  } while(strcmp(input, "exit")!=0);
}

/*----------------------------------------------------------------------------*
 *  Name:         readline()                                                  *
 *  Params:       char* input                                                 *
 *  Purpose:      Reads a line of input from user in the shell and stores     *
 *                it in a buffer which is then returned to the user.          *
 *                                                                            *
 *  Params: None  |   Returns:  char* buf                                     *
/*----------------------------------------------------------------------------*/
char* readline()
{
  char* buf = malloc(32*sizeof(char));

  // Get username and print the command prompt.

  printf("$ ");

  // If there's no buffer, allocation failed somewhy.
  if(!buf)
  {
    printf("Err: char* readLine() -- 'allocation failed somewhy");
    // I know this is really bad. Especially if there are subthreads running.
    // I'll have to find a better solution than just doing exit(0) here.
    exit(0);
  }

  // Get line of input from the user and place it in the buffer.
  fgets(buf, 32, stdin);
  // Remove the '\n' from the string and take only prior chars.
  buf[strcspn(buf, "\n")]=0;

  return buf; 
}

/*----------------------------------------------------------------------------*
 *  Name:         launch()                                                    *
 *  Params:       char* command                                               *
 *  Purpose:      Creates the appropriate environment for a fork, forks the   *
 *                original program, and then execs the proper command. A lot  *
 *                this code is pretty much exactly as it appears in-lecture.  *
 *  Params: char* command  |   Returns:  None                                 *
/*----------------------------------------------------------------------------*/
void launch(char* command)
{
  int childProcess();                   // Just a function prototype.

  pid_t pid, finished_pid;              // Process ID's of the child process.
  int status, exit_status;              // Status of the child process.

  // Create a process fork here, if we can. 
  pid = fork();
  switch(pid)
  {
    case -1:
      printf("Error forking the process for the first child.\n");
      exit(1);
      break;
    case 0:
      printf("\tFork was successful. First child code now starting.\n");
      // Call the program that actually executes the child process.
      exit_status=childProcess(command);
      break;
    default:
      // Wait for any child to be finished.
      finished_pid = wait((int *)0);
      break;
  }
  // End of switch on first fork().
}

/*----------------------------------------------------------------------------*
 *  Name:         childProcess()                                              *
 *  Params:       char* command                                               *
 *  Purpose:      Accepts the command as a string and then uses exec to       *
 *                replace the current process space with the command.         *
 *  Params: char* command  |   Returns:  int exit_status                      *
/*----------------------------------------------------------------------------*/
int childProcess(char *command)
{
  // An 'if' tree is definitely a bad idea for this sort of thing.
  // A smarter, better program would parse the commands for arguments,
  // break it all into two variables, one for the command and one array
  // for the args, and then pass them all to this function which would
  // just exec the command and args[] variables. I don't really have any
  // time to implement that here right now. Which is unfortunate, because
  // I'd like to have this program accept arguments from the user in the shell.
  // I suppose this program will stay icky until I update it :(

  if(strcmp(command, "/bin/ls")==0)
  {
    execl(command, command, "-lh", NULL);
  }
  if(strcmp(command, "/bin/du")==0)
  {
    execl(command, command, "-sch", NULL);
  }
  if(strcmp(command, "/bin/df")==0)
  {
    execl(command, command, "-h", "-t", "ext4", NULL);
  }
}
/* She sells C shells by the C shore. */