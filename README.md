# Rshell - A basic command shell
Rshell, much like Bash, is a program that takes in the user input commands and executes them.
In fact, Rshell is a more user-friendly version of Bash with some modifications and limitations.
Please see the **Uses and Specifications** section first to understand how Rshell interprets commands
from the user and how to use it effectively. Then please see the **Bugs, Limitations, and Errors** section
to see what my program cannot support or properly carry out.

All bugs, errors, fixes, or modifications can be sent to ykamo001@ucr.edu.
Reported bugs and errors will be handled as soon as possible and submitted for correctness.
Modifications will be implemented based on evaluations of effectiveness.

## Installing and Running
To get Rshell and run it, please open up a terminal and run these commands:
```
git clone https://github.com/ykamo001/rshell.git
cd rshell/
make
bin/rshell
```

## Uses and Specifications
1. To exit Rshell, simply enter in the command `exit` whenever you wish to exit.

2. Supports the connectors `;` `&&` and `||`.
	* Commands separated by `;` will be executed regardless of whether the previous command was successful or not.
	* Commands separated by `&&` will only execute if the last command was successful.
	* Commands separated by `||` will only execute if the last command failed.

	An example of precedence: `ls -a && rmdir folder || mkdir new_folder; touch main.cpp; mv main.cpp new_folder`

	Rshell will interpret this command as such:`(ls -a && rmdir folder) || (mkdir new_folder); touch main.cpp; mv main.cpp new_folder`
	
	Thus only if both `ls -a` and `rmdir folder` are successful will `mkdir new_folder` _not_ execute, otherwise if
	any of the first two fail, a new directory called "new_folder" will be made. As for the next two commands, 
	they will execute regardless of whatever happened before.

3. Anything typed after the comment identifier `#` will not be registered as commands and will be considered as comments.
	
	An example of how Rshell interprets comments: `ls -a && touch file.cpp #anything I type now will not be executed; cat file.cpp; exit`
	
	Rshell will filter out this command to: `ls -a && touch file.cpp`, and thus literally _everything_ after the `#` does not
	get recognized by Rshell as a command. 
	If you must write comments, please do so at the very end of you entire command line, as such:
	`ls -a && touch file.cpp ; cat file.cpp; exit #anything I type now will not be executed`

## Bugs, Limitations, and Errors
### Rshell
1. Rshell does not differentiate between `&&` and `&`, and between `||` and `|`.

2. Rshell  does not support i/o command such as `echo "Hello World" >> file.cpp` and will cause the program to terminate and crash.

3. After pressing `ctrl-z`, Rshell will stop properly and be pushed to the background; however, after returning to Rshell
by using `fg`, Rshell will not output a command line or prompt, but it is still running; press `enter` once to see prompt.

4. If accidentally inserted more connectors, such as `&&&&&` or `||||||`, Rshell will interpret this as a user mistake and consider 
them as `&&` and `||`, respectively.

5. When using `echo`, if connectors are combined improperly with other connectors or symbols, such as: `echo hello &| echo world` or
`echo hello &\ echo world` will cause the program to terminate and crash.

6. You will be able to recursively call Rshell within Rshell, resulting in multiple Rshells running Rshells. Executing the `exit` command
will simply make you exit the current Rshell you are running; therefore, if you ran Rshell, then ran Rshell again, and ran Rshell one more
time, in order to exit Rshell completely, you would need to execute `exit`, followed by `exit`, now placing you in the initial Rshell you
called, and finally executing `exit` one last time. This can lead to Rshell-ception and is not recommended, as you can easily lose track
of which Rshell you are in.

### ls
7. Running `ls` with the `-l` and/or `R` flag may cause unexpected output of 
`do_ypcall: clnt_call: RPC: Unable to send; errno = Operation not permitted` to randomly appear. Program will still run and output
everything as expected, except this, too, shall be there.

8. `ls` cannot output the files, directories, executables, and hidden files with specific colors.

9. If any error is occurred with any directory or file, `ls` will terminate and output a specific error message indicating where things
went wrong.

10. If there is a soft link to another file, when running `ls -l` there will be no indication of which file it points to, i.e. 
you will not see the output of `-> filename`.

### Piping and I/O Redirection
11. Solely for I/O redirection, order of commands entered will not matter. Program will effectively separate inputted command and execute 
all input redirection first, i.e `<`, then will implement all output redirection, i.e `>`, and finally run all `>>` commands. This may,
depending on the inputted command, cause result to be not as expected or executed like bash.

12. Adding excessive white-space in the command will increase the chances of the program **NOT** crashing or producing error.

13. The command `echo < file1 file2 file3` will definitely cause the program to abort and produce errors. This can be fixed by placing any
extra amount of white-space anywhere in the command after `echo` and before `file3`. If any command causes program to abort, using more 
white-space may fix error.

14. Running the command `cat > file1` will land you in an infinite loop to enter whatever you like to be put into `file1`, and can be ended
by `ctrl+c`, but will also cause `rshell` to terminate. 

15. Running big commands will cause program to produce memory map error and may abort, depending on the length and command.

16. Piping and I/O both use extensive heap allocation and may cause memory errors to occur. 

17. If an I/O command fails, then it will not execute any command connector by `||`, the program will think it is a success; therefore
even if an I/O command failed, it will go on to execute the commands connected by `&&`, as it is a success. 

18. Adding comments to the I/O command line will cause program to terminate, but is not affected in piping stand-alone, or if I/O
is implemented with piping.

### Signals
19. `^C` while in use of `cat` will also append an empty line to the end of the file

20. `^C` with big, complex commands might cause zombie processes to remain while running `rshell`

21. `^Z` will send `rshell` to the background
