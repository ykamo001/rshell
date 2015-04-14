#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>
#include <cstring>
#include <vector>
#include <boost/algorithm/string/trim.hpp>

using namespace std;

int main()
{
	string user = getlogin();	//gets the username and stores it in string user
	char name[100];				//initialize emptry string to be used to get the host name
	gethostname(name, 100);		//gets the host's name and stores it in the char array variable name
	
	bool done = false;
	string command;
	char *token;
	vector<char*> library;
	vector<string> sc_cmd;
	while(!done)
	{
		cout << user << "@" << name << " $ ";	//output user name, @ symbol, the host name, followed by $
							// output at this point should result in: [user]@[host]$
		getline(cin, command);				//get the entire line of command from user
		if(command == "exit")
		{
			done = true;
		}
		else
		{
			char cmd[command.size()];
			strcpy(cmd, command.c_str());
			token = strtok(cmd, ";");
			while(token != NULL)
			{
				library.push_back(token);
				token = strtok(NULL, ";");
			}
			for(int i = 0; i < library.size(); ++i)
			{
				sc_cmd.push_back(string(library.at(i)));
			}
			for(int i = 0; i < sc_cmd.size(); ++i)
			{
				cout << sc_cmd.at(i) << endl;
			}
		}
	}
	return 0;
}
