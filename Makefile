#Makefile

all: test data

data :
	mkdir ./data

test: main.c model.c
	gcc main.c model.c -o test

clean:
	rm -f *.o
	rm -f test
	rm -f -r ./data

