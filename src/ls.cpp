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
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <set>
#include <iomanip>
#include <queue>
#include <stack>

using namespace std;
using namespace boost;

//next three bools used for ease of access to flags
bool hasa = false;	//holds flag for a	
bool hasl = false;	//holds flag for l
bool hasR = false;	//hold flag for R

bool compareNoCase(const string& s1, const string& s2)
{		//takes care of capital letter comparisons
	return strcasecmp( s1.c_str(), s2.c_str() ) <= 0;
}

void make_strings(int size, char** parse, vector<string> &give)
{	//converts char** argv into vector of strings for easier use
	for(int i = 1; i < size; ++i)
	{
		give.push_back(string(parse[i]));
	}
}

void identify(vector<string> &commands, bool &cond)
{
	vector<string> replace;
	for(unsigned int i = 0; i < commands.size(); ++i)
	{
		if(commands.at(i).at(0) == '-')
		{
			if(commands.at(i).size() > 1)
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
			}
			else
			{
				cond = false;
			}
		}	//this function identifies where the directories are in the command line
		else
		{
			replace.push_back(commands.at(i));
		}	//determines what the flags are, and checks if they are all valid
	}
	commands = replace;
}

void getfiles(vector<string> &currfiles, char *temp)
//this function gets all the files and directories
{				//from whatever directory it is pointed to, and stores them in a vector of strings
	DIR *currdir;
	struct dirent *files;
	errno = 0;
	if(NULL == (currdir = opendir(temp)))
	{
		perror("There was an error with opendir() ");
		exit(1);
	}
	while(NULL != (files = readdir(currdir)))
	{
		if(hasa)
		{
			currfiles.push_back(string(files->d_name));
		}
		else
		{
			if(string(files->d_name).at(0) != '.')
			{
				currfiles.push_back(string(files->d_name));
			}
		}
	}
	if(errno != 0)
	{
		perror("There was an error with readdir() ");
		exit(1);
	}
	if(-1 == closedir(currdir))
	{
		perror("There was an error with closedir() ");
		exit(1);
	}
	sort(currfiles.begin(), currfiles.end(), compareNoCase);
}

void outputnorm(vector<string> &display)
{	//outputs the files/directories 
	unsigned int width = 0;
	for(unsigned int i = 0; i < display.size(); ++i)
	{
		if(display.at(i).size() > width)
		{
			width = display.at(i).size();
		}
	}
	unsigned int total = 0;
	for(unsigned int i = 0; i < display.size(); ++i)
	{
		total += width;
		if(total <= 80)
		{
			cout.width(width);
			cout << left <<  display.at(i) << "   ";
		}
		else
		{
			cout << endl;
			cout.width(width);
			cout << left << display.at(i) << "   ";
			total = 0;
		}
	}
	cout << endl;
}

void getpath(const vector<string> &take, vector<string> &give, string &org, string &absname) 
{		
	string temp;
	bool fromhome = true;
	if(org.size() < 5)
	{
		fromhome = false;
	}
	else
	{
		for(unsigned int k = 0; k < 5; ++k)
		{
			if(org.at(k) != absname.at(k))
			{
				fromhome = false;
			}
		}
	}
	if(!fromhome)
	{
		temp = absname;
		for(unsigned int i = 0; i < take.size(); ++i)
		{
			absname.append(org);
			absname.append("/");
			absname.append(take.at(i));
			give.push_back(absname);
			absname = temp;
		}
	}
	else
	{
		temp = org;
		for(unsigned int i = 0; i < take.size(); ++i)
		{
			org.append("/");
			org.append(take.at(i));
			give.push_back(org);
			org = temp;
		}
	}
}

void getabsolName(string &name)
{
	char *ptr = new char[1024];
	if(NULL == (getcwd(ptr, 1024)))
	{
		perror("There was an error with getcwd() ");
		exit(1);
	}
	strcat(ptr, "/");
	name = string(ptr);
	delete []ptr;
}

void outputl(vector<string> &files, string &pname, string &absname)
{
	struct passwd *userid;
	struct group *groupid;
	struct stat status;
	vector<string> path;
	getpath(files, path, pname, absname);
	int total = 0;
	for(unsigned int i = 0; i < path.size(); ++i)
	{
		if(-1 == (stat(path.at(i).c_str(), &status)))
		{
			perror("There was an error with stat() ");
			exit(1);
		}
		else
		{
			total += status.st_blocks;
		}
	}
	cout << "Total: " << total/2 << endl;
	for(unsigned int i = 0; i < path.size(); ++i)
	{
		if(-1 == (stat(path.at(i).c_str(), &status)))
		{
			perror("There was an error with stat() ");
			exit(1);
		}
		else
		{
			if(S_IFDIR & status.st_mode)
			{
				cout << 'd';
			}
			else if(S_ISLNK(status.st_mode))
			{
				cout << 'l';
			}
			else
			{
				cout << '-';
			}
			cout << ((S_IRUSR & status.st_mode) ? 'r' : '-');
			cout << ((S_IWUSR & status.st_mode) ? 'w' : '-');
			cout << ((S_IXUSR & status.st_mode) ? 'x' : '-');
			cout << ((S_IRGRP & status.st_mode) ? 'r' : '-');
			cout << ((S_IWGRP & status.st_mode) ? 'w' : '-');
			cout << ((S_IXGRP & status.st_mode) ? 'x' : '-');
			cout << ((S_IROTH & status.st_mode) ? 'r' : '-');
			cout << ((S_IWOTH & status.st_mode) ? 'w' : '-');
			cout << ((S_IXOTH & status.st_mode) ? 'x' : '-');
			cout <<	' ' << status.st_nlink << ' ';
			if(NULL == (userid = getpwuid(status.st_uid)))
			{
				perror("There was an error with getpwuid() ");
				exit(1);
			}
			else
			{
				cout << userid->pw_name << ' ';
			}
			if(NULL == (groupid = getgrgid(status.st_gid)))
			{
				perror("There was an error with getgrdid() ");
				exit(1);
			}
			else
			{
				cout << groupid->gr_name << ' ';
			}
			cout << setw(7) << right << status.st_size << ' ';
			struct tm *tm;
			char timebuf[15];
			if(NULL == (tm = localtime(&(status.st_mtime))))
			{
				perror("There was an error with localtime() ");
				exit(1);
			}
			else
			{
				strftime(timebuf, 15, "%b %d %H:%M", tm);
				cout << timebuf << ' ';
			}
			cout << files.at(i);
		}
		cout << endl;
	}
}

void doR(vector<string> files, string parent, string absname)
{
	struct stat status;
	vector<string> filesnow;
	for(unsigned int i = 0; i < files.size(); i++)
	{
		if((files.at(i) != "." && files.at(i) !="..") && (hasa || (files.at(i).at(0) == '.' && files.at(i).at(1) =='/') || files.at(i).at(0) !='.'))
		{
			string pathname = parent + '/' + files.at(i);
			if(-1 == stat(pathname.c_str(), &status))
			{
				perror("There was an error with stat() ");
				exit(1);
			}
			if(S_IFDIR & status.st_mode)
			{
				cout << pathname << ":" << endl;
				getfiles(filesnow, const_cast<char*>(pathname.c_str()));
				if(hasl)
				{
					outputl(filesnow, pathname, absname);
					cout << endl;
				}
				else
				{
					outputnorm(filesnow);
					cout << endl;
				}
				doR(filesnow, pathname, absname);
			}
		}
		filesnow.clear();
	}
}

int main(int argc, char **argv)
{
	string absname;
	getabsolName(absname);
	vector<string> commands;
	vector<string> dirfiles;
	bool okay = true;
	make_strings(argc, argv, commands);		//first change all inputs into strings
	identify(commands, okay);		//organize and get all the info
	if(okay)	//if no errors in flag, proceed to output
	{
		if(commands.size() ==  0)	//if directories were not specified, come here
		{
			commands.push_back(".");
		}
		for(unsigned int i = 0; i < commands.size(); ++i)
		{
			getfiles(dirfiles, const_cast<char*>(commands.at(i).c_str()));
			if(commands.size() > 1)
			{
				cout << commands.at(i) << ": " << endl;
			}
			if(!hasl && !hasR)	//if no l or R flag, simple case, do this
			{
				outputnorm(dirfiles);
			}
			else if(hasl && !hasR)	//has the l flag but not R
			{
				outputl(dirfiles, commands.at(i), absname);
			}
			else if(hasR)	//has the R flag
			{
				if(hasl)
				{
					if(commands.size() <= 1)
					{
						cout << commands.at(i) << ":" << endl;
					}
					outputl(dirfiles, commands.at(i), absname);
				}
				else
				{
					if(commands.size() <= 1)
					{
						cout << commands.at(i) << ":" << endl;
					}
					outputnorm(dirfiles);
					cout << endl;
				}
				doR(dirfiles, commands.at(i), absname);
			}
			if(i < (commands.size() - 1))
			{
				cout << endl;
			}
			dirfiles.clear();
		}
	}
	else
	{
		cout << "Error: flag not recognized or valid. " << endl;
	}
	return 0;
}
