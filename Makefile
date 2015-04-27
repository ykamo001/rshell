all:
	if [ ! -d bin ]; then mkdir bin; fi
	g++ -Wall -Werror -ansi -pedantic src/main.cpp -o bin/rshell
	g++ -Wall -Werror -ansi -pedantic src/cp.cpp
rshell:
	if [ ! -d bin ]; then mkdir bin; fi
	g++ -Wall -Werror -ansi -pedantic src/main.cpp

cp:
	if [ ! -d bin ]; then mkdir bin; fi
	g++ -Wall -Werror -ansi -pedantic src/cp.cpp
