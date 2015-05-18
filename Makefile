all:
	if [ ! -d bin ]; then mkdir bin; fi
	g++ -std=c++11 -Wall -Werror -pedantic ./src/rshell.cpp -o bin/rshell
	g++ -Wall -Werror -ansi -pedantic src/cp.cpp -o bin/cp
	g++ -Wall -Werror -ansi -pedantic src/ls.cpp -o bin/ls

rshell:
	if [ ! -d bin ]; then mkdir bin; fi
	g++ -std=c++11 -Wall -Werror -ansi -pedantic src/rshell.cpp -o bin/rshell

cp:
	if [ ! -d bin ]; then mkdir bin; fi
	g++ -Wall -Werror -ansi -pedantic src/cp.cpp -o bin/cp

ls:
	if [ ! -d bin ]; then mkdir bin; fi
	g++ -Wall -Werror -ansi -pedantic src/ls.cpp -o bin/ls
