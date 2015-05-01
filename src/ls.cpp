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
#include <sys/stat.h>
#include <fcntl.h> 
#include <cstdlib>
#include <errno.h>
#include <dirent.h>

using namespace std;
using namespace boost;

bool hasa = false;	
bool hasl = false;	
bool hasR = false;

void make_strings(int size, char** parse, vector<string> &give)	//converts char** argv into vector of strings for easier use
{
	for(int i = 1; i < size; ++i)
	{
		give.push_back(string(parse[i]));
	}
}

void identify(const vector<string> &commands, vector<int> &dirs , bool &cond)
{
	for(unsigned int i = 0; i < commands.size(); ++i)
	{
		if(commands.at(i).at(0) == '-')
		{
			for(unsigned int j = 1; j < commands.at(i).size(); ++j)
			{
				if((commands.at(i).at(j) == 'a') && !hasa) 
				{
					hasa = true;
				}
				else if((commands.at(i).at(j) == 'l') && !hasl)
				{
					hasl = true;
				}
				else if((commands.at(i).at(j) == 'R') && !hasR)
				{
					hasR = true;
				}
				else if((commands.at(i).at(j) != 'R') && (commands.at(i).at(j) != 'a') && (commands.at(i).at(j) != 'l'))
				{
					cond = false;
				}
			}
		}	//this function identifies where the directories are in the command line
		else
		{
			dirs.push_back(i+1);
		}	//determines what the flags are, and checks if they are all valid
	}
}

void getfiles(vector<string> &currfiles, char** argv, const int &loc)	//this function gets all the files and directories
{				//from whatever directory it is pointed to, and stores them in a vector of strings
	DIR *currdir;
	struct dirent *files;
	errno = 0;
	if(NULL == (currdir = opendir(argv[loc])))
	{
		perror("There was an error with opendir(). ");
		exit(1);
	}
	while(NULL != (files = readdir(currdir)))
	{
		currfiles.push_back(string(files->d_name));
	}
	if(errno != 0)
	{
		perror("There was an error with readdir(). ");
		exit(1);
	}
	if(-1 == closedir(currdir))
	{
		perror("There was an error with closedir(). ");
		exit(1);
	}
	sort(currfiles.begin(), currfiles.end());
}

void outputnorm(vector<string> &display)
{
	int row = 0;
	if(hasa)
	{
		for(unsigned int i = 0; i < display.size(); ++i)
		{
			if(row >= 7)
			{
				cout << endl;
				row = 0;
			}
			else
			{
				row++;
			}
			cout << display.at(i) << "  ";
		}
	}
	else
	{
		for(unsigned int i = 0; i < display.size(); ++i)
		{
			if(row >= 7)
			{
				cout << endl;
				row = 0;
			}
			else
			{
				row++;
			}
			if(display.at(i).at(0) != '.')
			{	
				cout << display.at(i) << "  ";
			}
		}
	}
	cout << endl;
}

int main(int argc, char **argv)
{
	vector<string> commands;
	vector<int> directories;
	vector<string> dirfiles;
	bool okay = true;
	make_strings(argc, argv, commands);		//first change all inputs into strings
	identify(commands, directories, okay);		//organize and get all the info
	if(okay)	//if no errors in flag, proceed to output
	{
		if(directories.size() > 0)
		{
			if(!hasl && !hasR)
			{
				for(unsigned int i = 0; i < directories.size(); ++i)
				{
					getfiles(dirfiles, argv, directories.at(i));
					if(directories.size() > 1)
					{
						cout << commands.at(directories.at(i)-1) << ": " << endl;
					}
					outputnorm(dirfiles);
					if(i < directories.size()-1)
					{
						cout << endl;
					}
					dirfiles.clear();
				}
			}
		}
		else
		{
			char *temp[1];
			char root = '.';
			char *hold = &root;
			temp[0] = hold;
			if(!hasl && !hasR)
			{
				getfiles(dirfiles, temp, 0);
				outputnorm(dirfiles);
			}
		}
	}
	else
	{
		cout << "Error: flag not recognized or valid. " << endl;
	}
	return 0;
}
