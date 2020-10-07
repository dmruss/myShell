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


char* getPath(char* file, int local) {
  char* str1;
  char* str2;
  if (local) {
    str1 = "./";
  }else {
    str1 = "/usr/bin/";
  }

  str2 = file;
  char* path = (char*) malloc(1 + strlen(str1) + strlen(str2));
  strcpy(path, str1);
  strcpy(path, str2);
  return path;

}

void execRedirect(char* path, char* argv[], char* reArgv[], int length) {
  //char* reArgv[80];
  char* openPath = getPath(argv[length - 1], 1);
  /*int i = 0;
  while (argv[i] != ">" || argv[i] != "<") {
    reArgv[i] = argv[i];
    i++;
  }
  reArgv[i] = NULL;*/
  //get function descriptor for file

  //check for input redirection
  if (!strcmp(argv[length - 2], "<")) {
    int input_fds = open(openPath, O_RDONLY);
    if(dup2(input_fds, STDIN_FILENO) < 0) {
      printf("Unable to duplicate file descriptor.");
      exit(EXIT_FAILURE);
    }

    execvp(path, reArgv);
  }
  //check for output redirection
  else if (!strcmp(argv[length - 2], ">")) {
    int output_fds = open(openPath, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
    if(dup2(output_fds, STDOUT_FILENO) < 0) {
      printf("Unable to duplicate file descriptor.");
      exit(EXIT_FAILURE);
    }
    execvp(path, reArgv);
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

      argv[i] = commands[i];
      length += 1;

     }
  argv[length] = NULL;
  return hasRedirect;
}

void forkAndExec(char* path, char* argv[], char* reArgv[], int length, int hasRedirect) {
  int pid = fork();

  if (pid < 0) { //error forking process

      cout << "Error\n";
        return;
        }

  else if (pid == 0) { //child process
      //cout << "Child process" << '\n';

      //test file redirection//////////////////////////////////////////////////

      if (hasRedirect) {


      execRedirect(path, argv, reArgv, length);
      //create file path to local file
     /*char* first;
     char* second;
     first = "./";
     second = argv[length - 1];
     char* openPath = (char*) malloc(1 + strlen(first) + strlen(second));
     strcpy(openPath, first);
     strcpy(openPath, second);*/


     /*char* openPath = getPath(argv[length - 1], 1);



     //get function descriptor for file


     //check for input redirection
     if (!strcmp(argv[length - 2], "<")) {
       int input_fds = open(openPath, O_RDONLY);
       if(dup2(input_fds, STDIN_FILENO) < 0) {
         printf("Unable to duplicate file descriptor.");
         exit(EXIT_FAILURE);
       }

       execvp(path, reArgv);




     }
     //check for output redirection
     else if (!strcmp(argv[length - 2], ">")) {
       int output_fds = open(openPath, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
       if(dup2(output_fds, STDOUT_FILENO) < 0) {
         printf("Unable to duplicate file descriptor.");
         exit(EXIT_FAILURE);
       }


       execvp(path, reArgv);
     }*/
   }

      /////////////////////////////////////////////////////////////////////////

      //check if command is not cd or help, then execute command
      if (strcmp(argv[0], "cd")  || strcmp(argv[0], "help")) {
          if (execvp(path, argv)) { //execute command and check for failure
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
               "Other Unix system calls may be possible but untested\n"
               "ls [OPTION]... [FILE]...\n"
               "pwd [OPTION]...\n"
               "whoami [OPTION]...\n"
               "hostname [OPTION]...\n"
               "ps [options]\n"
               "cd [directory]\n"
               "file redirection with '>' and '<'\n"
               "exit\n"
               "type [cmd] --help for more information\n");
    }
    sleep(1);
    cout <<'\n';

        }



}



int main()
{
    //clear console when shell is opened
    system("clear");

    //command vector and command string
    vector<char*> commands;
    char* cmd = " ";



    while (1) {

      int hasRedirect = 0;

	    //initialize
      initialize();
	    commands.clear();

	    //get input
      commands = getInput();
	    cmd = commands[0];

      //check for exit or quit
	    if (!strcmp(cmd, "exit") || !strcmp(cmd, "quit") || !strcmp(cmd, "q")) {
		      return 0;
	       }


	    //create file path//FUNCTION + str1 option for redirect or no
	    /*char* str1;
	    char* str2;
	    str1 = "/usr/bin/";
	    str2 = cmd;
	    char* path = (char*) malloc(1 + strlen(str1) + strlen(str2));
	    strcpy(path, str1);
	    strcpy(path, str2);*/

      char* path = getPath(cmd, 0);


	    //build argument list//FUNCTION
	    char* argv[80];
      char* reArgv[80];
	    int length = 0;

      hasRedirect = buildArgList(argv, reArgv, length, commands);

      /*
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

		      argv[i] = commands[i];
		      length += 1;

	       }
	    argv[length] = NULL;
*/


    //create child process to handle command//FUNCTION
    /*int pid = fork();

   	if (pid < 0) { //error forking process

        cout << "Error\n";
		      return 1;
		      }

    else if (pid == 0) { //child process
		    //cout << "Child process" << '\n';

        //test file redirection//////////////////////////////////////////////////

        if (hasRedirect) {


        execRedirect(path, argv, reArgv, length);
        //create file path to local file
       /*char* first;
       char* second;
       first = "./";
       second = argv[length - 1];
       char* openPath = (char*) malloc(1 + strlen(first) + strlen(second));
       strcpy(openPath, first);
       strcpy(openPath, second);*/


       /*char* openPath = getPath(argv[length - 1], 1);



       //get function descriptor for file


       //check for input redirection
       if (!strcmp(argv[length - 2], "<")) {
         int input_fds = open(openPath, O_RDONLY);
         if(dup2(input_fds, STDIN_FILENO) < 0) {
           printf("Unable to duplicate file descriptor.");
           exit(EXIT_FAILURE);
         }

         execvp(path, reArgv);




       }
       //check for output redirection
       else if (!strcmp(argv[length - 2], ">")) {
         int output_fds = open(openPath, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
         if(dup2(output_fds, STDOUT_FILENO) < 0) {
           printf("Unable to duplicate file descriptor.");
           exit(EXIT_FAILURE);
         }


         execvp(path, reArgv);
       }*/

       forkAndExec(path, argv, reArgv, length, hasRedirect);
     }

        /////////////////////////////////////////////////////////////////////////

        //check if command is not cd or help, then execute command
		   /* if (strcmp(argv[0], "cd")  || strcmp(argv[0], "help")) {
		        if (execvp(path, argv)) { //execute command and check for failure
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
                 "Other Unix system calls may be possible but untested\n"
                 "ls [OPTION]... [FILE]...\n"
                 "pwd [OPTION]...\n"
                 "whoami [OPTION]...\n"
                 "hostname [OPTION]...\n"
                 "ps [options]\n"
                 "cd [directory]\n"
                 "file redirection with '>' and '<'\n"
                 "exit\n"
                 "type [cmd] --help for more information\n");
      }
		  sleep(1);
		  cout <<'\n';

		      }



	}*/
    return 0;
}
