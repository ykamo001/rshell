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
using namespace boost;

bool done = false;

void orfinder(string filter, vector<string> &take)
{
	unsigned int start = 0;
	unsigned int cut = 0;
	bool finished = false;
	bool complete = false;
	bool over = false;
	string copy = filter;
	while(!finished)
	{
		for(unsigned int i = start; i <= filter.size()-2 && !complete; ++i)
		{
			if((filter.at(i) == '|') && (filter.at(i+1) == '|'))
			{
				copy = copy.substr(start, cut);
				trim(copy);
				take.push_back(copy);
				while(i < filter.size() && filter.at(i) == '|')
				{
					++i;
				}
				if(i == filter.size())
				{
					over = true;
				}
				else
				{
					start = i;
				}
				complete = true;
			}
			else
			{
				cut++;
			}
		}
		copy = filter;
		if(!complete || over)
		{
			copy = copy.substr(start, copy.size()-start);
			trim(copy);
			take.push_back(copy);
			finished = true;
		}
		else
		{
			cut = 0;
			complete = false;
			over = false;
		}
	}
}

bool charfinder(const char element, const string search)
{
	char last = 0;
	bool condition = false;
	for(unsigned int i = 0; i <= search.size()-2; ++i)
	{
		if(search.at(i) == element && last != element && search.at(i+1) != element)
		{
			condition = true;
		}
		if(i == search.size()-2)
		{
			last = search.at(i);
			if(search.at(i+1) == element && last != element)
			{
				condition = true;
			}
		}
		last = search.at(i);
	}
	return condition;
}

void finder(const string search, bool &haspipe, bool &hasleft, bool &has2right, bool &hasright)
{
	bool complete = false;
	for(unsigned int i = 0; i <= search.size()-2 && !complete; ++i)
	{
		if((search.at(i) == '>') && (search.at(i+1) == '>'))
		{
			has2right = true;
			complete = true;
		}
	}
	hasleft = charfinder('<', search);
	hasright = charfinder('>', search);
	haspipe = charfinder('|', search);
}

void normalBash(string command)
{
	char *token;
	vector<string> sc_cmd;		//sc_cmd stands for semicolon_commands - this holds the commands in string form
	vector<string> copy;		//this will hold the new commands from sc_cmd without any comments for a short while
	vector<string> and_cmd;		//this will hold the string of single commands combined by &&
	vector<string> or_cmd;		//this will hold the string of commands combined by ||
	char* cmd = new char[command.size()];
	if(command.size() > 0)
	{
		copy.push_back(command);
	}
	if(copy.size() > 0)
	{
		strcpy(cmd, (copy.at(0)).c_str()); 
		copy.clear();
		token = strtok(cmd, ";");
		while(token != NULL)
		{
			sc_cmd.push_back(string(token)); //parse the string, looking for semicolons to split the commands
			token = strtok(NULL, ";");
		}
		for(unsigned int i = 0; i < sc_cmd.size(); ++i)
		{
			strcpy(cmd, (sc_cmd.at(i)).c_str());
			token = strtok(cmd, "#");		//parse the string looking for the start of a comment
			if(token != NULL)		//then only add the beginning part of the command and throw away everything after the #
			{
				if(string(token).find_first_not_of(' ') != std::string::npos) //if the parsed string is only white spaces
				{									//then dont add it list of commands
					copy.push_back(string(token));	
				}
			}
		}
		sc_cmd.clear();
		for(unsigned int i = 0; i < copy.size(); ++i)
		{
			sc_cmd.push_back(copy.at(i));	//move the new filtered commands into the proper vector
		}
		copy.clear();
		for(unsigned int i = 0; i < sc_cmd.size(); ++i)	//keep executing these commands regardless of result from previous command
		{
			trim(sc_cmd.at(i));		//remove all the unnecessary white spaces around the string if in there
			orfinder(sc_cmd.at(i), or_cmd);
			unsigned int fails = 0;
			for(unsigned int i = 0; i < or_cmd.size(); ++i)	//keep doing || commands as long as the last command failed
			{
				trim(or_cmd.at(i));
				if(fails == i)
				{
					strcpy(cmd, (or_cmd.at(i)).c_str());
					token = strtok(cmd, "&&");
					while(token != NULL)	//parse the string command for && and split the code down to single commands
					{
						and_cmd.push_back(string(token));
						token = strtok(NULL, "&&");
					}
					unsigned int trues = 0;
					for(unsigned int i = 0; i < and_cmd.size(); ++i) //keep doing && commands as long as the last command succeeded
					{
						trim(and_cmd.at(i));
						bool haspipe = false;
						bool hasleft = false;
						bool has2right = false;
						bool hasright = false;
						finder(and_cmd.at(i), haspipe, hasleft, has2right, hasright);
						if(trues == i)
						{
							strcpy(cmd, (and_cmd.at(i)).c_str());
							token = strtok(cmd, " ");
							while(token != NULL)	//break down the command into strings
							{
								copy.push_back(string(token));
								token = strtok(NULL, " ");
							}
							char **argv = new char*[copy.size()+1];	//create an array of char pointers
							for(unsigned int j = 0; j < copy.size(); ++j)
							{
								trim(copy.at(j));	//add char pointers into an array of pointers
								argv[j] = const_cast<char*>((copy.at(j)).c_str());	//this will allow us to use execvp 
							}
							argv[copy.size()] = 0;
							int pid = fork();
							if(pid == -1)
							{
								perror("There was an error with fork() ");
							}
							else if(pid == 0)
							{
								if(and_cmd.at(i) != "exit")	//only move along to execvp if the command is not exit
								{
									if(haspipe || hasleft || has2right || hasright)
									{
										cout << "I/O redirection" << endl;
										_exit(0);
									}
									else
									{	
										if(-1 == execvp((copy.at(0)).c_str(), argv))
										{
											perror("There was an error with execvp() ");	
											_exit(1);	//if the command failed, then kill the child process and exit
										}
									}
								}
								else //if the command is to exit, dont do anything, simply kill the child process and exit
								{
									_exit(1);
								}
							}
							else
							{
								int status;
								wait(&status);
								if(status == -1)
								{
									perror("There was an error with wait() ");
								}
								else if(status == 0)	//if the last command was executed successfully, increase the counter
								{
									++trues;
								}
								if(and_cmd.at(i) == "exit")	//if the user entered the exit command, end the shell and program
								{
									delete []argv;		//since we are done with the program early, we must delete
									delete []cmd;		//dynamic memory from leaking out
									copy.clear();
									and_cmd.clear();
									or_cmd.clear();
									sc_cmd.clear();
									done = true;	
									return;
								}
							}
							delete []argv;	//make sure to delete the dynamically allocated memory
						}
						copy.clear();	//clear out the vector to be used again
					}	//end of and_cmd
					if(trues != and_cmd.size())
					{
						fails++;   //if the amount of commands that were successfully executed is not the same as the
							//number of commands executed, then increase the number of commands failed
					}
				}
				and_cmd.clear();	//clear out the commands in and vector since we are done
			}   //end of or_cmd
			or_cmd.clear();	 //clear out the commands in the or vector since we are done
		}	//end of sc_cmd
		sc_cmd.clear();	//clear out the commands in the semicolon vector since we are done
	}
	delete []cmd;//make sure to delete the dynamically allocated memory
}

int main()
{
	char *user = getlogin();	//gets the user name
	string command;
	if(user == NULL)
	{
		perror("There was an error with getlogin() ");
	}
	char name[100];				//initialize empty string to be used to get the host name
	int check = gethostname(name, 100);		//gets the host's name and stores it in the char array variable name
	if(check == -1)
	{
		perror("There was en error with gethostname() ");
	}
	while(!done)		//loop until user enters exit and the loop is terminated
	{
		if((user != NULL) && (check == 0))
		{
			cout << user << "@" << name << " $ ";	//output user name, @ symbol, the host name, followed by $
		}			// output at this point should result in: [user]@[host]$
		else
		{
			cout << "$ ";
		}
		getline(cin, command);	//get the entire line of command from user
		unsigned int loc = command.find_first_of("#");	//keep only everything before the first comment mark
		command = command.substr(0, loc);
		if(command == "exit")
		{
			done = true;	//if the command they type in is initially exit
					//then there is no need to continue traversing the program, simply end
		}
		else
		{
			normalBash(command);
		}
	}
	return 0;
}
