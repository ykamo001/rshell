#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>
#include <cstring>
#include <wait.h>
#include <vector>
#include <fcntl.h>
#include <cstdlib>
#include <errno.h>
#include <sys/stat.h>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string.hpp>

using namespace std;
using namespace boost;

bool done = false;

void onlyleft(string command, bool hasright, bool haspipe, string master)
{
	char *token;
	char* cmd = new char[command.size()];
	strcpy(cmd, command.c_str());
	token = strtok(cmd, "<");
	vector<string> holder;
	while(token != NULL)
	{
		if(string(token).find_first_not_of(' ') != string::npos)
		{
			holder.push_back(string(token));
		}
		token = strtok(NULL, "<");
	}
	delete []cmd;
	int savestdin;
	if(!haspipe)
	{
		if(-1 == (savestdin = dup(0)))
		{
			perror("There was an error with dup(). ");
			exit(1);
		}
		if(-1 == close(0))
		{
			perror("There was an error with close(). ");
			exit(1);
		}
	}
	int savestdout;
	int write_to;
	if(!haspipe)
	{
		if(hasright)
		{
			if(-1 == (savestdout = dup(1)))
			{
				perror("There was an error with dup(). ");
				exit(1);
			}
			if(-1 == close(1))
			{
				perror("There was an error with close(). ");
				exit(1);
			}
			if(-1 == (write_to = open(master.c_str(), O_CREAT | O_TRUNC | O_APPEND | O_WRONLY, S_IRUSR | S_IWUSR)))
			{
				perror("There was an error with open() .");
				exit(1);
			}
		}
	}
	vector<vector<string> > all_cmd;
	vector<string> to_use;
	for(unsigned int i = 0; i < holder.size(); ++i)
	{
		trim(holder.at(i));
		cmd = new char[(holder.at(i)).size()];
		strcpy(cmd, (holder.at(i)).c_str());
		token = strtok(cmd, " ");
		while(token != NULL)
		{
			to_use.push_back(string(token));
			token = strtok(NULL, " ");
		}
		all_cmd.push_back(to_use);
		to_use.clear();
		delete []cmd;
	}
	if(all_cmd.size() == 1)
	{
		cerr << "rshell: syntax error " << endl;
		exit(1);
	}
	unsigned int determine = 0;
	bool special_case = false;
	bool has_command = false;
	bool run_once = false;
	for(unsigned int i = 0; i < all_cmd.size(); ++i)
	{
		to_use = all_cmd.at(i);
		if(i == 0)
		{
			if(to_use.size() > 1)
			{
				has_command = true;
			}
		}

		if(to_use.size() == 1)
		{
			determine++;
		}
	}
	if(determine == all_cmd.size())
	{
		special_case = true;
	}
	if((has_command) && (determine == all_cmd.size()-1))
	{
		run_once = true;
	}
	vector<string> all_info;
	if(!run_once)
	{
		for(unsigned int i = 0; i < all_cmd.size(); ++i)
		{
			to_use.clear();
			to_use = all_cmd.at(i);
			if(i == 0)
			{
				for(unsigned int j = 0; j < to_use.size(); ++j)
				{
					all_info.push_back(to_use.at(j));
				}
			}
			else if(special_case && (i == all_cmd.size()-1) && (to_use.size() == 1))
			{
				all_info.push_back(to_use.at(0));
			}
			else if(to_use.size() > 1)
			{
				for(unsigned int j = 1; j < to_use.size(); ++j)
				{
					all_info.push_back(to_use.at(j));
				}
			}
		}
		char **argv = new char*[all_info.size()+1];	//create an array of char pointers
		for(unsigned int k = 0; k < all_info.size(); ++k)
		{
			argv[k] = const_cast<char*>((all_info.at(k)).c_str());	//this will allow us to use execvp
		}
		argv[all_info.size()] = 0;
		int pid = fork();
		if(pid == -1)
		{
			perror("There was an error with fork(). ");
			exit(1);
		}
		else if(pid == 0)
		{
			if(-1 == execvp((all_info.at(0)).c_str(), argv))
			{
				perror("There was an error with execvp(). ");
				_exit(1);
			}
		}
		else
		{
			int status;
			wait(&status);
			if(status == -1)
			{
				perror("There was an error with wait(). ");
				exit(1);
			}
		}
		delete []argv;
	}
	else
	{
		all_info = all_cmd.at(0);
		char **argv = new char*[all_info.size()+1];	//create an array of char pointers
		for(unsigned int k = 0; k < all_info.size(); ++k)
		{
			argv[k] = const_cast<char*>((all_info.at(k)).c_str());	//this will allow us to use execvp 
		}
		argv[all_info.size()] = 0;
		int pid = fork();
		if(pid == -1)
		{
			perror("There was an error with fork(). ");
			exit(1);
		}
		else if(pid == 0)
		{
			if(-1 == execvp((all_info.at(0)).c_str(), argv))
			{
				perror("There was an error with execvp(). ");
				_exit(1);
			}
		}
		else
		{
			int status;
			wait(&status);
			if(status == -1)
			{
				perror("There was an error with wait(). ");
				exit(1);
			}
		}
		delete []argv;
	}
	if(!haspipe)
	{
		if(hasright)
		{
			if(-1 == close(write_to))
			{
				perror("There was an error with close(). ");
				exit(1);
			}
			if(-1 == dup2(savestdout, 1))
			{
				perror("There was an error with dup2(). ");
				exit(1);
			}
		}
		if(-1 == dup2(savestdin, 0))
		{
			perror("There was an error with dup2(). ");
			exit(1);
		}
	}
}

string master_finder(string command)
{
	char *token;
	char* cmd = new char[command.size()];
	strcpy(cmd, command.c_str());
	token = strtok(cmd, ">");
	vector<string> holder;
	while(token != NULL)
	{
		if(string(token).find_first_not_of(' ') != string::npos)
		{
			holder.push_back(string(token));
		}
		token = strtok(NULL, ">");
	}
	delete []cmd;
	vector<vector<string> > all_cmd;
	vector<string> to_use;
	for(unsigned int i = 0; i < holder.size(); ++i)
	{
		trim(holder.at(i));
		char* cmd = new char[holder.at(i).size()];
		strcpy(cmd, (holder.at(i)).c_str());
		token = strtok(cmd, " ");
		while(token != NULL)
		{
			to_use.push_back(string(token));
			token = strtok(NULL, " ");
		}
		all_cmd.push_back(to_use);
		to_use.clear();
		delete []cmd;
	}
	string temp = all_cmd.at(all_cmd.size()-1).at(0);
	return temp;
}

void onlyright(string command, bool hasleft, bool has2right, bool haspipe)
{
	int savestdin;
	if(haspipe)
	{
		if(-1 == (savestdin = dup(0)))
		{
			perror("There was an error with dup(). ");
			exit(1);
		}
		if(-1 == close(0))
		{
			perror("There was an error with close(). ");
			exit(1);
		}
	}
	char *token;
	char* cmd = new char[command.size()];
	strcpy(cmd, command.c_str());
	token = strtok(cmd, ">");
	vector<string> holder;
	while(token != NULL)
	{
		if(string(token).find_first_not_of(' ') != string::npos)
		{
			holder.push_back(string(token));
		}
		token = strtok(NULL, ">");
	}
	delete []cmd;
	vector<vector<string> > all_cmd;
	vector<string> to_use;
	for(unsigned int i = 0; i < holder.size(); ++i)
	{
		trim(holder.at(i));
		char* cmd = new char[holder.at(i).size()];
		strcpy(cmd, (holder.at(i)).c_str());
		token = strtok(cmd, " ");
		while(token != NULL)
		{
			to_use.push_back(string(token));
			token = strtok(NULL, " ");
		}
		all_cmd.push_back(to_use);
		to_use.clear();
		delete []cmd;
	}
	if(all_cmd.size() == 1)
	{
		cerr << "rshell: syntax error " << endl;
		exit(1);
	}
	string master = all_cmd.at(all_cmd.size()-1).at(0);
	to_use.clear();
	bool special_case = false;
	bool single_special = false;
	unsigned int determine = 0;
	for(unsigned int i = 0; i < all_cmd.size(); ++i)
	{
		to_use = all_cmd.at(i);
		if(i == 0)
		{
			if(to_use.size() == 1)
			{
				determine++;
				if(to_use.at(i) == "cat")
				{
					single_special = true;
				}
			}
		}
		else
		{
			if(to_use.size() == 1)
			{
				determine++;
			}
		}
		to_use.clear();
	}
	if(determine == all_cmd.size())
	{
		special_case = true;
	}
	holder.clear();
	for(unsigned int i = 0; i < all_cmd.size(); ++i)
	{
		to_use.clear();
		to_use = all_cmd.at(i);
		if(i == 0)
		{
			if(special_case && single_special)
			{
				string inputs;
				int savestdout;
				if(-1 == (savestdout = dup(1)))
				{
					perror("There was an error with dup(). ");
					exit(1);
				}
				if(-1 == close(1))
				{
					perror("There was an error with close(). ");
					exit(1);
				}
				int write_to;
				if(hasleft || has2right)
				{
					if(-1 == (write_to = open(master.c_str(), O_CREAT | O_APPEND | O_WRONLY, S_IRUSR | S_IWUSR)))
					{
						perror("There was an error with open() .");
						exit(1);
					}
				}
				else
				{
					if(-1 == (write_to = open(master.c_str(), O_CREAT | O_TRUNC | O_APPEND | O_WRONLY, S_IRUSR | S_IWUSR)))
					{
						perror("There was an error with open() .");
						exit(1);
					}
				}
				while(special_case)
				{
					getline(cin, inputs);
					inputs += '\n';
					if(-1 == write(write_to, inputs.c_str(), inputs.size()))
					{
						perror("There was an error with write(). ");
						exit(1);
					}
				}
				if(-1 == close(write_to))
				{
					perror("There was an error with close(). ");
					exit(1);
				}
				if(-1 == dup2(savestdout, 1))
				{
					perror("There was an error with dup2(). ");
					exit(1);
				}
			}
			else
			{
				for(unsigned int q = 0; q < to_use.size(); ++q)
				{
					holder.push_back(to_use.at(q));
				}
			}
		}
		else
		{
			to_use = all_cmd.at(i);
			for(unsigned int k = 0; k < to_use.size(); ++k)
			{
				int read_from;
				if(k == 0)
				{
					if(i < all_cmd.size()-1)
					{	
						if(-1 == (read_from = open((to_use.at(k)).c_str(), O_CREAT | O_TRUNC | O_RDONLY, S_IRUSR | S_IWUSR)))
						{
							perror("There was an error with open(). ");
							exit(1);
						}
						if(-1 == close(read_from))
						{
							perror("There was an error with close(). ");
							exit(1);
						}
					}
				}
				else
				{
					if(-1 == (read_from = open((to_use.at(k)).c_str(), O_RDONLY, S_IRUSR | S_IWUSR)))
					{
						perror("There was an error with open(). ");
						exit(1);
					}
					if(-1 == close(read_from))
					{
						perror("There was an error with close(). ");
						exit(1);
					}
					holder.push_back(to_use.at(k));
				}
			}
		}
	}
	char **argv = new char*[holder.size()+1];	
	for(unsigned int j = 0; j < holder.size(); ++j)
	{
		trim(holder.at(j));	
		argv[j] = const_cast<char*>((holder.at(j)).c_str());	
	}
	argv[holder.size()] = 0;
	int savestdout;
	if(-1 == (savestdout = dup(1)))
	{
		perror("There was an error with dup(). ");
		exit(1);
	}
	if(-1 == close(1))
	{
		perror("There was an error with close(). ");
		exit(1);
	}
	int write_to;
	if(hasleft || has2right)
	{
		if(-1 == (write_to = open(master.c_str(), O_CREAT | O_APPEND | O_WRONLY, S_IRUSR | S_IWUSR)))
		{
			perror("There was an error with open() .");
			exit(1);
		}
	}
	else
	{
		if(-1 == (write_to = open(master.c_str(), O_CREAT | O_TRUNC | O_APPEND | O_WRONLY, S_IRUSR | S_IWUSR)))
		{
			perror("There was an error with open() .");
			exit(1);
		}
	}
	int pid = fork();
	if(pid == -1)
	{
		perror("There was an error with fork(). ");
		exit(1);
	}
	else if(pid == 0)
	{
		if(-1 == execvp((holder.at(0)).c_str(), argv))
		{
			perror("There was an error with execvp(). ");
			_exit(1);
		}
	}
	else
	{
		int status;
		wait(&status);
		if(status == -1)
		{
			perror("There was an error with wait(). ");
			exit(1);
		}
		if(-1 == close(write_to))
		{
			perror("There was an error with close(). ");
			exit(1);
		}
	}
	delete []argv;
	if(-1 == dup2(savestdout, 1))
	{
		perror("There was an error with dup2(). ");
		exit(1);
	}
	if(haspipe)
	{
		if(-1 == dup2(savestdin, 0))
		{
			perror("There was an error with dup2(). ");
		}
	}
}

bool file_exists(string &name)
{
	FILE *file;
	if(NULL == (file = fopen(name.c_str(), "r")))
	{
		return false;
	}
	else
	{
		fclose(file);
		return true;
	}
}

void left_right_seperate(string command, string &left, string &right, string &master, string &final)
{
	bool finish = false;
	string command_to_use;
	for(unsigned int i = 0; i < command.size()-1 && !finish; ++i)
	{
		if((command.at(i) == '<') || (command.at(i) == '>') || ((command.at(i) == '>') && (command.at(i+1) == '>')))
		{
			command_to_use = command.substr(0, i);
			finish = true;
		}
	}
	char *token;
	char* cmd = new char[command_to_use.size()];
	strcpy(cmd, (command_to_use).c_str());
	vector<string> temp;
	vector<string> left_commands;
	vector<string> right_commands;
	token = strtok(cmd, " ");
	while(token != NULL)
	{
		temp.push_back(string(token));
		token = strtok(NULL, " ");
	}
	delete []cmd;
	for(unsigned int i = 0; i < temp.size(); ++i)
	{
		if(!file_exists(temp.at(i)))
		{
			final += temp.at(i);
			final += " ";
		}
	}
	temp.clear();
	cmd = new char[command.size()];
	strcpy(cmd, command.c_str());
	token = strtok(cmd, "<");
	while(token != NULL)
	{
		temp.push_back(string(token));
		token = strtok(NULL, "<");
	}
	delete []cmd;
	for(unsigned int i = 0; i < temp.size(); ++i)
	{
		cmd = new char[(temp.at(i)).size()];
		strcpy(cmd, (temp.at(i)).c_str());
		token = strtok(cmd, ">");
		bool first = true;
		while(token != NULL)
		{
			if(first)
			{
				left_commands.push_back(string(token));
				first = false;
			}
			else
			{
				right_commands.push_back(string(token));
			}
			token = strtok(NULL, "<");
		}
		delete []cmd;
	}
	for(unsigned int i = 0; i < left_commands.size(); ++i)
	{
		left += left_commands.at(i);
		if(i != left_commands.size()-1)
		{
			left += " < ";
		}
	}
	right += final;
	right += " > ";
	for(unsigned int i = 0; i < right_commands.size(); ++i)
	{
		right += right_commands.at(i);
		right += ' ';
		if(i != right_commands.size()-1)
		{
			right += '>';
		}
		right += ' ';
	}
	master = master_finder(right);
}

void orfinder(string filter, vector<string> &take, char match)
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
			if((filter.at(i) == match) && (filter.at(i+1) == match))
			{
				copy = copy.substr(start, cut);
				trim(copy);
				take.push_back(copy);
				while(i < filter.size() && filter.at(i) == match)
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

void double_right_parse(string command, vector<string> &doubleright, string &word)
{
	vector<string> divided;
	orfinder(command, divided, '>');
	vector<string> rest;
	rest.push_back(divided.at(0));
	for(unsigned int i = 1; i < divided.size(); ++i)
	{
		string temp = divided.at(i);
		bool finish = false;
		bool over = false;
		for(unsigned int j = 0; j < temp.size() && !finish; ++j)
		{
			if((temp.at(j) == '>') || (temp.at(j) == '<'))
			{
				temp = temp.substr(0, j);
				doubleright.push_back(temp);
				temp = divided.at(i);
				temp = temp.substr(j, temp.size()-j);
				rest.push_back(temp);
				finish = true;
			}
			if(j == temp.size()-1)
			{
				over = true;
			}
		}
		if(over && !finish)
		{
			doubleright.push_back(temp);
		}
	}
	for(unsigned int i = 0; i < rest.size(); ++i)
	{
		word += rest.at(i);
	}
}

void otherBash(string command, bool hasleft, bool hasright, bool has2right, bool haspipe)
{
	if(hasright && !hasleft && !has2right)
	{
		onlyright(command, hasleft, has2right, haspipe);
	}
	else if(hasleft && !hasright && !has2right)
	{
		onlyleft(command, hasright, haspipe, "");
	}
	else if(hasleft && hasright && !has2right)
	{
		string left;
		string right;
		string master_file;
		string do_not_need;
		left_right_seperate(command, left, right, master_file, do_not_need);
		onlyleft(left, hasright, haspipe, master_file);
		onlyright(right, hasleft, has2right, haspipe);
	}
	else if(has2right && !hasleft && !hasright)
	{
		onlyright(command, hasleft, has2right, haspipe);
	}
	else if((hasleft || hasright) && has2right)
	{
		vector<string> doubleright;
		string word;
		double_right_parse(command, doubleright, word);
		string left;
		string right;
		string master_file;
		string need_command;
		left_right_seperate(word, left, right, master_file, need_command);
		need_command += " >> ";
		for(unsigned int m = 0; m < doubleright.size(); ++m)
		{
			need_command += doubleright.at(m);
			if(m < doubleright.size()-1)
			{
				need_command += " >> ";
			}
		}
		if(hasleft && !hasright)
		{
			onlyleft(left, hasright, haspipe, master_file);
			onlyright(need_command, hasleft, has2right, haspipe);
		}
		else if(!hasleft && hasright)
		{
			onlyright(right, hasleft, false, haspipe);
			need_command += " ";
			need_command += master_file;
			onlyright(need_command, hasleft, has2right, haspipe);
		}
		else
		{
			onlyleft(left, hasright, haspipe, master_file);
			onlyright(right, hasleft, has2right, haspipe);
			need_command += " ";
			need_command += master_file;
			onlyright(need_command, hasleft, has2right, haspipe);
		}
	}
}

void piping(string command)
{
	vector<int> PIDS;
	char *token;
	char* cmd = new char[command.size()];
	vector<string> parsed;
	strcpy(cmd, command.c_str());
	token = strtok(cmd, "|");
	while(token != NULL)
	{
		parsed.push_back(string(token));
		token = strtok(NULL, "|");
	}
	delete []cmd;
	int savestdout;
	if(-1 == (savestdout = dup(1)))
	{
		perror("There was an error with dup(). ");
		exit(1);
	}
	const int PIPE_READ = 0;
	const int PIPE_WRITE = 1;
	struct pipe_holder {int fd[2]; };
	vector<pipe_holder> pipes;
	for(unsigned int i = 0; i < parsed.size(); ++i)
	{
		int fd[2];
		pipe_holder fdHold;
		if(-1 == pipe(fd))
		{
			perror("There was an error with pipe(). ");
			exit(1);
		}
		for(unsigned int j = 0; j < 2; ++j)
		{
			fdHold.fd[j] = fd[j];
		}
		pipes.push_back(fdHold);
	}
	for(unsigned int i = 0; i < parsed.size(); ++i)
	{
		int status = 0;
		int pid = fork();
		PIDS.push_back(pid);
		bool keep_going = false;
		if(-1 == pid)
		{
			perror("There was an error with fork(). ");
			exit(1);
		}
		else if(pid == 0)
		{
			if(i == 0)
			{
				if(-1 == dup2(pipes[i].fd[PIPE_WRITE], 1))
				{
					perror("There was an error with dup2(). ");
					exit(1);
				}
				if(-1 == close(pipes[i].fd[PIPE_READ]))
				{
					perror("There was an error with close(). ");
					exit(1);
				}
			}
			else if((i > 0) && ((i+1) < parsed.size()))
			{
				if(-1 == dup2(pipes[i].fd[PIPE_WRITE], 1))
				{
					perror("There was an error with dup2(). ");
					exit(1);
				}
				if(-1 == close(pipes[i].fd[PIPE_READ]))
				{
					perror("There was an error with close(). ");
					exit(1);
				}
				if(-1 == dup2(pipes[i-1].fd[PIPE_READ], 0))
				{
					perror("There was an error with dup2(). ");
					exit(1);
				}
				if(-1 == close(pipes[i-1].fd[PIPE_WRITE]))
				{
					perror("There was an error with close(). ");
					exit(1);
				}
			}
			else if((i > 0) && ((i+1) >= parsed.size())) 
			{
				if(-1 == dup2(savestdout, 1))
				{
					perror("There was an error with dup2(). ");
					exit(1);
				}
				if(-1 == dup2(pipes[i-1].fd[PIPE_READ], 0))
				{
					perror("There was an error with dup2(). ");
					exit(1);
				}
				if(-1 == close(pipes[i-1].fd[PIPE_WRITE]))
				{
					perror("There was an error with close(). ");
					exit(1);
				}
			}
			bool haspipe = false;
			bool hasleft = false;
			bool has2right = false;
			bool hasright = false;
			finder(parsed.at(i), haspipe, hasleft, has2right, hasright);
			haspipe = true;
			if(hasleft || hasright || has2right)
			{
				otherBash(parsed.at(i), hasleft, hasright, has2right, haspipe);
			}
			else
			{
				vector<string> temp;
				cmd = new char[(parsed.at(i)).size()];
				strcpy(cmd, (parsed.at(i)).c_str());
				token = strtok(cmd, " ");
				while(token != NULL)
				{
					temp.push_back(string(token));
					token = strtok(NULL, " ");
				}
				delete []cmd;
				char **argv = new char*[temp.size()+1];	//create an array of char pointers
				for(unsigned int j = 0; j < temp.size(); ++j)
				{
					trim(temp.at(j));	//add char pointers into an array of pointers
					argv[j] = const_cast<char*>((temp.at(j)).c_str());	//this will allow us to use execvp 
				}
				argv[temp.size()] = 0;
				if(-1 == execvp((temp.at(0)).c_str(), argv))
				{
					perror("There was an error with execvp(). ");
					exit(1);
				}
				delete []argv;
			}
			_exit(1);
		}
		else
		{
			if(i != 0)
			{
				if(-1 == close(pipes[i-1].fd[PIPE_READ]))
				{
					perror("There was an error with close(). ");
					exit(1);
				}
				if(-1 == close(pipes[i-1].fd[PIPE_WRITE]))
				{
					perror("There was an error with close(). ");
					exit(1);
				}
			}
			if((i+1) < parsed.size())
			{
				keep_going = true;
			}
			else if(((i+1) >= parsed.size()) && !keep_going)
			{
				for(unsigned int y = 0; y < PIDS.size(); ++y)
				{
					if(-1 == waitpid(PIDS[y], &status, WUNTRACED))
					{
						perror("There was an error with waitpid(). ");
						exit(1);
					}
				}
			}
		}
	}
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
	delete []cmd;
	if(copy.size() > 0)
	{
		cmd = new char[(copy.at(0)).size()];
		strcpy(cmd, (copy.at(0)).c_str()); 
		copy.clear();
		token = strtok(cmd, ";");
		while(token != NULL)
		{
			sc_cmd.push_back(string(token)); //parse the string, looking for semicolons to split the commands
			token = strtok(NULL, ";");
		}
		delete []cmd;
		for(unsigned int i = 0; i < sc_cmd.size(); ++i)
		{
			cmd = new char[(sc_cmd.at(i)).size()];
			strcpy(cmd, (sc_cmd.at(i)).c_str());
			token = strtok(cmd, "#");		//parse the string looking for the start of a comment
			if(token != NULL)		//then only add the beginning part of the command and throw away everything after the #
			{
				if(string(token).find_first_not_of(' ') != std::string::npos) //if the parsed string is only white spaces
				{									//then dont add it list of commands
					copy.push_back(string(token));	
				}
			}
			delete []cmd;
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
			orfinder(sc_cmd.at(i), or_cmd, '|');
			unsigned int fails = 0;
			for(unsigned int i = 0; i < or_cmd.size(); ++i)	//keep doing || commands as long as the last command failed
			{
				trim(or_cmd.at(i));
				if(fails == i)
				{
					cmd = new char[(or_cmd.at(i)).size()];
					strcpy(cmd, (or_cmd.at(i)).c_str());
					token = strtok(cmd, "&&");
					while(token != NULL)	//parse the string command for && and split the code down to single commands
					{
						and_cmd.push_back(string(token));
						token = strtok(NULL, "&&");
					}
					delete []cmd;
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
							cmd = new char[(and_cmd.at(i)).size()];
							strcpy(cmd, (and_cmd.at(i)).c_str());
							token = strtok(cmd, " ");
							while(token != NULL)	//break down the command into strings
							{
								copy.push_back(string(token));
								token = strtok(NULL, " ");
							}
							delete []cmd;
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
									if(haspipe || hasleft || hasright || has2right)
									{
										if(!haspipe)
										{	
											otherBash(and_cmd.at(i), hasleft, hasright, has2right, haspipe);
											_exit(0);
										}
										else
										{
											piping(and_cmd.at(i));
											_exit(0);
										}
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
									delete []argv;		//since we are done with the program early, we must delete memory
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
