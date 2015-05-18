#include <iostream>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

using namespace std;

volatile int c_cnt = 0;
volatile int s_cnt = 0;
volatile int z_cnt = 0;

void handle_signal(int userSig)
{
	if(userSig == SIGINT)
	{
		cout << 'c' << endl;
		c_cnt++;
		sleep(1);
	}
	else if(userSig == SIGQUIT)
	{
		cout << '\\' << endl;
		s_cnt++;
		sleep(1);
	}
	else if(userSig == SIGTSTP)
	{
		cout << 's' << endl;
		z_cnt++;
		if(z_cnt == 3)
		{
			cout << "^C count: " << c_cnt << endl;
			cout << "^\\_count: " << s_cnt << endl;
			cout << "^Z count: " << z_cnt << endl;
			if(-1 == kill(0, SIGKILL))
			{
				perror("There was an error with kill(). ");
			}
		}
		else
		{
			if(-1 == kill(0, SIGSTOP))
			{
				perror("There was an error with kill(). ");
			}
		}
	}
}

int main()
{
	struct sigaction sig;
	sig.sa_flags = SA_SIGINFO;
	sig.sa_handler = handle_signal;
	
	if(-1 == sigaction(SIGINT, &sig, NULL))
	{
		perror("There was an error with sigaction(). ");
	}
	if(-1 == sigaction(SIGQUIT, &sig, NULL))
	{
		perror("There was an error with sigaction(). ");
	}
	if(-1 == sigaction(SIGTSTP, &sig, NULL))
	{
		perror("There was an error with signal(). ");
	}
	while(1)
	{
		cout << 'x' << endl;
		sleep(1);
	}
	return 0;
}
