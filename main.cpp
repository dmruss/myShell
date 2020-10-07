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


	    //create file path
	    char* str1;
	    char* str2;
	    str1 = "/usr/bin/";
	    str2 = cmd;
	    char* path = (char*) malloc(1 + strlen(str1) + strlen(str2));
	    strcpy(path, str1);
	    strcpy(path, str2);


	    //build argument list
	    char* argv[80];
      char* reArgv[80];
	    int length = 0;
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

    //create child process to handle command
    int pid = fork();

   	if (pid < 0) { //error forking process

        cout << "Error\n";
		      return 1;
		      }

    else if (pid == 0) { //child process
		    //cout << "Child process" << '\n';

        //test file redirection//////////////////////////////////////////////////
        int number1, number2, sum;

        //create file path to local file
       char* first;
       char* second;
       first = "./";
       second = argv[length - 1];
       char* openPath = (char*) malloc(1 + strlen(first) + strlen(second));
       strcpy(openPath, first);
       strcpy(openPath, second);




       //get function descriptor for file
       int input_fds = open(openPath, O_RDONLY);

       //check for input
       if (!strcmp(argv[length - 2], "<")) {
         if(dup2(input_fds, STDIN_FILENO) < 0) {
           printf("Unable to duplicate file descriptor.");
           exit(EXIT_FAILURE);
         }

         //char* aCmd[] = {"sort", NULL};

         execvp(path, reArgv);

         //scanf("%d %d", &number1, &number2);
         //sum = number1 + number2;
         //printf("%d + %d = %d\n", number1, number2, sum);


       }
       //check for output
       else {
         cout << "Does not support output yet\n";
       }





        if (hasRedirect) {
          cout << "There is a redirect\n";
          cout << "file name: " << argv[length - 1] << '\n';
        }
        /////////////////////////////////////////////////////////////////////////

        //check if command is not cd
		    if (strcmp(argv[0], "cd")) {
		        if (execvp(path, argv)) { //execute command and check for failure
			           cout << "Unrecognized/unsupported command\n";
		             }
		        }

        exit(1);
      }

    else { //parent process

      //check for cd command and execute
	    if (!strcmp(argv[0], "cd")) {
			     chdir(argv[1]);
		    }

      //check for help commands
      if(!strcmp(argv[0], "help")) {
            puts("Welcome to my shell.  Here is a list of supported commands.\n"
                 "Other Unix system calls may be possible but untested\n"
                 "ls\n"
                 "pwd\n"
                 "whoami\n"
                 "hostname\n"
                 "ps\n"
                 "cd\n"
                 "file redirection with '>' and '<'\n"
                 "exit\n");
      }
		  sleep(1);
		  //cout << "Parent process" << '\n';

		      }



	}
    return 0;
}
