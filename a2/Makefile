all : simpletest mytest tests 

tests : test1 test2 

test1: 
	simpletest 

test2: 
	mytest

simpletest : simpletest.o smalloc.o testhelpers.o
	gcc -Wall -g -o simpletest simpletest.o smalloc.o testhelpers.o

mytest : mytest.o smalloc.o testhelpers.o
	gcc -Wall -g -o mytest mytest.o smalloc.o testhelpers.o
	
%.o : %.c smalloc.h
	gcc -Wall -g -c $<
	
clean : 
	rm simpletest *.o
