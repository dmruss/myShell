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



char* getPath(char* cmd) {
	char* str1;
	char* str2;
	str1 = "/usr/bin/";
	cout << str1 << '\n';
	str2 = cmd;
	char* path = (char*) malloc(1 + strlen(str1) + strlen(str2));
	strcpy(path, str1);
	strcpy(path, str2);
	cout << path << '\n';
	return path;
}

char** getCommands(vector<char*> commands) {
	char* cStringCommands[commands.size()];
	for (int i = 0; i < commands.size(); i++) {
		cStringCommands[i] = commands[i];
	}
	cStringCommands[commands.size()-1] = NULL;
	return cStringCommands;
}

int main()
{
    string clear = "clear";
    //char* path = "/usr/bin/";
    system("clear");

    vector<char*> commands;
    commands.push_back("go");
    char* cmd = " ";
    int shouldRun = 1;

    while (shouldRun) {

	//initialize
    	initialize();
	commands.clear();

	//get input
        commands = getInput();
	cmd = commands[0];
	//check for exit
	if (!strcmp(cmd, "exit")) {
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


	//char* path = (char*) malloc(100);
	//path = getPath(cmd);
	//char* iPath = "/usr/bin/pwd";

	//char* argv[100];
	//argv = getCommands(commands);


	//build argument list
    	//char *argv[] = {"pwd", "--help",  NULL};
	char* argv[80];
	int length = 0;
	for (int i = 0; i < commands.size(); i++) {
		argv[i] = commands[i];
		length += 1;

	}
	argv[length] = NULL;




	if (!strcmp(argv[0], "cd")) {
			chdir(argv[1]);
			//char* yeah[] = {"pwd", NULL};
			//execvp("/usr/bin/pwd", yeah);
			//continue;
		}







	int pid = fork();

   	if (pid < 0) { //error forking process
		cout << "Error\n";
		return 1;
		}
        else if (pid == 0) { //child process
		int fail = 0;
		//cout << "Child process" << '\n';

		if (strcmp(argv[0], "cd")) {
		//dup2("list.txt", 1);
			fail = execvp(path, argv);
		}
		if (fail) {
			cout << "Unrecognized/unsupported command\n";
		}
		exit(1);
		}
        else { //parent process
		//sleep(1);
		//continue;
		sleep(1);
		//cout << "Parent process" << '\n';

		}



	}
    return 0;
}
