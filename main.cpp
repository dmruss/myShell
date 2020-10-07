#include <iostream>
#include <string.h>
#include <stdio.h>
#include <sstream>
#include <vector>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <cstring>
#include <fcntl.h>
#include <sys/wait.h>



using namespace std;

void initialize() {
  cout << "my_shell>>";
}


vector<char*> getInput() {
  string command;
  vector<char*> parsedCommands;

  getline(cin, command);

  stringstream line(command);
  string intermediate;

  while(getline(line, intermediate, ' '))
  {
    char *cstr = new char[intermediate.length() + 1];
    strcpy(cstr, intermediate.c_str());
    parsedCommands.push_back(cstr);

  }
  return parsedCommands;
}


char* getPath(char* file, char* local) {
  char* str1;
  char* str2;
  if (!strcmp(local, "localFilePath")) {
    str1 = "./";
  }
  else if (!strcmp(local, "commandPath")) {
    str1 = "/usr/bin/";
  }

  str2 = file;
  char* path = (char*) malloc(1 + strlen(str1) + strlen(str2));
  strcpy(path, str1);
  strcpy(path, str2);
  return path;

}

void execRedirect(char* pathToCommand, char* argv[], char* reArgv[], int length) {

  char* pathToFile = getPath(argv[length - 1], "commandPath");

  //check for input redirection
  if (!strcmp(argv[length - 2], "<")) {
    int input_fds = open(pathToFile, O_RDONLY);
    if(dup2(input_fds, STDIN_FILENO) < 0) {
      printf("Unable to duplicate file descriptor.");
      exit(EXIT_FAILURE);
    }

    execvp(pathToCommand, reArgv);
  }
  //check for output redirection
  else if (!strcmp(argv[length - 2], ">")) {
    int output_fds = open(pathToFile, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
    if(dup2(output_fds, STDOUT_FILENO) < 0) {
      printf("Unable to duplicate file descriptor.");
      exit(EXIT_FAILURE);
    }
    execvp(pathToCommand, reArgv);
  }
}

int buildArgList(char* argv[], char* reArgv[], int &length, vector<char*> commands) {
  int hasRedirect = 0;
  for (int i = 0; i < commands.size(); i++) {
    //check for redirect
    if(!strcmp(commands[i], "<") || !strcmp(commands[i], ">")) {
        hasRedirect = 1;
        //build left side of argument list
        for (int i = 0; i < length; i++) {
          reArgv[i] = argv[i];
        }
        reArgv[length] = NULL;
    }
      //build full argument list
      argv[i] = commands[i];
      length += 1;

     }
  argv[length] = NULL;
  return hasRedirect;
}

void forkAndExec(char* argv[], char* reArgv[], int length, int hasRedirect) {
  char* pathToCommand = getPath(argv[0], "commandPath");
  int pid = fork();

  if (pid < 0) { //error forking process

      cout << "Error\n";
        return;
        }

  else if (pid == 0) { //child process
      //cout << "Child process" << '\n';

      if (hasRedirect) {
        execRedirect(pathToCommand, argv, reArgv, length);

        }

      //check if command is not cd or help, then execute command
      if (strcmp(argv[0], "cd")  || strcmp(argv[0], "help")) {
          if (execvp(pathToCommand, argv)) { //execute command and check for failure
               cout << "Unrecognized/unsupported command\n";
               }
          }

      exit(1);
    }

  else { //parent process

    //check for cd command and execute//FUNCTION
    if (!strcmp(argv[0], "cd")) {
         chdir(argv[1]);
      }

    //check for help commands//FUNCTION
    if(!strcmp(argv[0], "help")) {
          puts("Welcome to my shell.  Here is a list of supported commands.\n"
               "Other Unix system calls may be possible, but are untested\n\n"
               "ls [OPTION]... [FILE]...\n"
               "pwd [OPTION]...\n"
               "date\n"
               "whoami [OPTION]...\n"
               "hostname [OPTION]...\n"
               "ps [options]\n"
               "cd [directory]\n"
               "file redirection with '>' and '<'\n"
               "exit\n"
               "type [cmd] --help for more information\n");
    }

    wait(NULL);
    cout <<'\n';
        }
}

void clearConsole() {
    char* path = "/usr/bin/clear";
    char* argv[] = {"clear", NULL};
    char* reArgv[] = {};
    forkAndExec(argv, reArgv, 1, 0);
    puts("my_shell started.  Type 'help' for commands\n");
}



int main()
{

    clearConsole();

    //command vector and command string
    vector<char*> commands;
    int hasRedirect;

    while (1) {

	    //initialize
      char* argv[80]; //command and all arguments
      char* reArgv[80];  //command and all arguments to left of > if applicable
	    int argLength = 0;
      hasRedirect = 0;
      commands.clear();
      initialize();

      commands = getInput();

      //check for exit or quit
	    if (!strcmp(commands[0], "exit") || !strcmp(commands[0], "quit") || !strcmp(commands[0], "q")) {
		      return 0;
	       }

      hasRedirect = buildArgList(argv, reArgv, argLength, commands);

      forkAndExec(argv, reArgv, argLength, hasRedirect);
     }

    return 0;
}
