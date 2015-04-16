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
using namespace boost::algorithm;

int main()
{
	string user = getlogin();	//gets the user name and stores it in string user
	char name[100];				//initialize empty string to be used to get the host name
	gethostname(name, 100);		//gets the host's name and stores it in the char array variable name
	
	bool done = false;
	string command;
	char *token;
	vector<string> sc_cmd;				//sc_cmd stands for semicolon_commands - this holds the commands in string form
	vector<string> copy;			//this will hold the new commands from sc_cmd without any comments for a short while
	while(!done)							//loop until user enters exit and the loop is terminated
	{
		cout << user << "@" << name << " $ ";	//output user name, @ symbol, the host name, followed by $
							// output at this point should result in: [user]@[host]$
		getline(cin, command);				//get the entire line of command from user
		if(command == "exit")
		{
			done = true;					//if the command they type in is initially exit
							//then there is no need to continue traversing the program, simply end
		}
		else
		{
			char cmd[command.size()];
			strcpy(cmd, command.c_str());		//make the inputted command into a c string so that we can parse
			token = strtok(cmd, ";");
			while(token != NULL)
			{
				sc_cmd.push_back(string(token));		//parse the string, looking for semicolons to split the commands
				token = strtok(NULL, ";");
			}
			for(int i = 0; i < sc_cmd.size(); ++i)
			{
				strcpy(cmd, (sc_cmd.at(i)).c_str());
				token = strtok(cmd, "#");					//parse the string looking for the start of a comment
				if(token != NULL)		//then only add the beginning part of the command and throw away everything after the #
				{
					if(string(token).find_first_not_of(' ') != std::string::npos) //if the parsed string is only white spaces
					{															//then dont add it list of commands
						copy.push_back(string(token));	
					}
				}
			}
			sc_cmd.clear();
			for(int i = 0; i < copy.size(); ++i)
			{
				sc_cmd.push_back(copy.at(i));			//move the new filtered commands into the proper vector
			}
			copy.clear();
			cout << "Your commands are: " << endl;
			for(int i = 0; i < sc_cmd.size(); ++i)
			{
				trim(sc_cmd.at(i));					//remove all the unnecessary white spaces around the string if in there
				cout << sc_cmd.at(i) << endl;	
				if(sc_cmd.at(i) == "exit")			//if the command is exit, then end the function
				{
					done = true;
					return 0;
				}
			}
			sc_cmd.clear();
		}
	}
	return 0;
}
