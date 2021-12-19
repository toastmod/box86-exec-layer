all:
	gcc -std=c99 -o exec.so -shared exec.c -Wall -Wfatal-errors -fPIC -g -ldl
