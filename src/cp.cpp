#include <iostream>
#include <fstream>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h> 
#include <unistd.h>
#include <cstdlib>
#include <errno.h>
#include "Timer.h"

using namespace std;

void simple(char* input, char* output)
{
	int fdne;
	int fdold;
	if(-1 == (fdne = open(output, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR ))) 
	{
		perror("There was an error with open(). ");
		 exit(1);
	}
	if(-1 == (fdold = open(input, O_RDONLY))) 
	{
	    perror("There was an error with open(). ");
		 exit(1); 
	}
	int size;
	char c[BUFSIZ];
	if(-1 == (size = read(fdold, &c, sizeof(c)))) 
	{
		perror("There was an error with read(). ");
		exit(1);
	}
	while(size > 0)
	{
	   	if(-1 == write(fdne, c, size))
	   	{
		  	perror("There was an error with write(). ");
	       	exit(1);
	    }
	    if(-1 == (size = read(fdold, &c, sizeof(c)))) 
		{
		    perror("There was an error with read(). ");
		    exit(1);
		}
	}
	if(-1 == close(fdne))
	{
	    	perror("There was an error with close(). ");
	        	exit(1);
	}
	if(-1 == close(fdold))
	{
	    	perror("There was an error with close(). ");
	        	exit(1);
	}
}
void onechar(char* input, char* output)
{
	int fdne;
	int fdold;
	if(-1 == (fdne = open(output, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR ))) 
	{
		perror("There was an error with open(). ");
		 exit(1);
	}
	if(-1 == (fdold = open(input, O_RDONLY))) 
	{
	    perror("There was an error with open(). ");
		 exit(1); 
	}
	ifstream is(input);
	ofstream os(output);
	char c;
	while(is.get(c))
	{
		os.put(c);
	}
	if(-1 == close(fdne))
	{
	    	perror("There was an error with close(). ");
	        	exit(1);
	}
	if(-1 == close(fdold))
	{
	    	perror("There was an error with close(). ");
	        	exit(1);
	}
}
void rwonechar(char* input, char* output)
{
	int fdne;
	int fdold;
	if(-1 == (fdne = open(output, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR ))) 
	{
		perror("There was an error with open(). ");
		 exit(1);
	}
	if(-1 == (fdold = open(input, O_RDONLY))) 
	{
	    perror("There was an error with open(). ");
		 exit(1); 
	}
	int size;
	char c[1];
	if(-1 == (size = read(fdold, &c, sizeof(c)))) 
	{
		perror("There was an error with read(). ");
		exit(1);
	}
	while(size > 0)
	{
	   	if(-1 == write(fdne, c, size))
	   	{
		  	perror("There was an error with write(). ");
	       	exit(1);
	    }
	    if(-1 == (size = read(fdold, &c, sizeof(c)))) 
		{
		    perror("There was an error with read(). ");
		    exit(1);
		}
	}
	if(-1 == close(fdne))
	{
	    	perror("There was an error with close(). ");
	        	exit(1);
	}
	if(-1 == close(fdold))
	{
	    	perror("There was an error with close(). ");
	        	exit(1);
	}
}
int main(int argc, char *argv[])
{
	if(argc < 3)
	{
		cout << "Error: not enough arguments" << endl;
	}
	else
	{
		if(argc == 3)
		{
			int fdne;
			if(-1 == (fdne = open(argv[2], O_WRONLY | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR ))) 
			{
				perror("There was an error with open(). ");
				exit(1);
			}	
			else
			{
				simple(argv[1], argv[2]);
			}
		}
		else if(argc == 4)
		{
			Timer clk, clk1, clk2; 
			double wc, us, system = 0;
			
			clk.start();	
			onechar(argv[1], argv[2]);
			clk.elapsedWallclockTime(wc);
			clk.elapsedUserTime(us);
			clk.elapsedSystemTime(system);
			cout << "Wall time elapsed: " << wc << endl;
			cout << "User time elapsed: " << us << endl;
			cout << "System time elapsed: " << system << endl;
			
			clk1.start();
			rwonechar(argv[1], argv[2]);
			clk1.elapsedWallclockTime(wc);
			clk1.elapsedUserTime(us);
			clk1.elapsedSystemTime(system);
			cout << "Wall time elapsed: " << wc << endl;
			cout << "User time elapsed: " << us << endl;
			cout << "System time elapsed: " << system << endl;
			
			clk2.start();
			simple(argv[1], argv[2]);
			clk2.elapsedWallclockTime(wc);
			clk2.elapsedUserTime(us);
			clk2.elapsedSystemTime(system);
			cout << "Wall time elapsed: " << wc << endl;
			cout << "User time elapsed: " << us << endl;
			cout << "System time elapsed: " << system << endl;
		}
		else
		{
			cout << "Error: too many arguments. " << endl;
		}
	}
	return 0;
}
