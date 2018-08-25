
process: process3.o libstacklib.a
	gcc -o process -L . process3.o -lstacklib

process3.o: process3.c
	gcc -I . -c process3.c

libstacklib.a: libstacklibrary.o
	ar rc libstacklib.a libstacklibrary.o
	ranlib libstacklib.a

libstacklibrary.o: libstacklibrary.c
	gcc -c libstacklibrary.c -o libstacklibrary.o

clean:
	rm libstacklib.a process3.o libstacklibrary.o process
	ipcrm -a