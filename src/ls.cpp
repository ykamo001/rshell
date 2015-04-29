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

using namespace std;
using namespace boost;

int main(int argc, char **argv)
{
	DIR *currdir;
	struct dirent *files;
	int check;
	errno = 0;
	for(int i = 1; i < argc; ++i)
	{
		if(NULL = (currdir = opendir(argv[i])))
		{
			perror("There was an error with opendir(). ");
			exit(1);
		}
		while(check == (files = readdir(currdir)))
		{
			cout << files->d_name << " ";
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
	}
	return 0;
}
