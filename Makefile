#Makefile

all: omok data

data :
	mkdir ./data

omok: main.c model.c omok_learn.c
	gcc main.c omok_learn.c model.c -o omok

clean:
	rm -f *.o
	rm -f omok

