#Rshell - A basic command shell
Rshell, much like Bash, is a program that takes in the user input commands and executes them.
In fact, Rshell is a more user-friendly version of Bash with some modifications and limitations.
Please see the **Uses and Specifications** section first to understand how Rshell interprets commands
from the user and how to use it effectively. Then please see the **Bugs, Limitations, and Errors** section
to see what my program cannot support or properly carry out.

All bugs, errors, fixes, or modifications can be sent to ykamo001@ucr.edu.
Reported bugs and errors will be handled as soon as possible and submitted for correctness.
Modifications will be implemented based on evaluations of effectiveness.

##Installing and Running
To get Rshell and run it, please open up a terminal and run these commands:
```
git clone https://github.com/ykamo001/rshell.git
cd rshell/
make
bin/rshell
```

##Uses and Specifications
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

##Bugs, Limitations, and Errors
###Rshell
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

###ls
7. Running `ls` with the `-l` and/or `R` flag may cause unexpected output of 
`do_ypcall: clnt_call: RPC: Unable to send; errno = Operation not permitted` to randomly appear. Program will still run and output
everything as expected, except this, too, shall be there.

8. ls cannot output the files, directories, executables, and hidden files with specific colors.

9. If any error is occurred with any directory or file, `ls` will terminate and output a specific error message indicating where things
went wrong.

10. If there is a soft link to another file, when running `ls -l` there will be no indication of which file it points to, i.e. 
you will not see the output of `-> filename`.
