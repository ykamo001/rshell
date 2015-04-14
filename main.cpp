#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>

using namespace std;

int main()
{
	string user = getlogin();	//gets the username and stores it in string user
	char name[100];				//initialize emptry string to be used to get the host name
	gethostname(name, 100);		//gets the host's name and stores it in the char array variable name
	
	bool done = false;
	while(!done)
	{
		cout << user << "@" << name << " $ ";	//output user name, @ symbol, the host name, followed by $
							// output at this point should result in: [user]@[host]$
		string command;
		getline(cin, command);
		if(command == "exit")
		{
			done = true;
		}
	}
	return 0;
}
