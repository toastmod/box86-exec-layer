
test: exec64
	gcc -std=c99 -o test test.c
	export LD_PRELOAD=./exec.o

.PHONY: clean exec64 exec32

exec64:
	gcc -std=c99 -o exec.so -shared exec.c -Wall -Wfatal-errors -fPIC -g -ldl -DBOX64SYS

exec32:
	gcc -std=c99 -o exec.so -shared exec.c -Wall -Wfatal-errors -fPIC -g -ldl

clean:
	rm exec.so test