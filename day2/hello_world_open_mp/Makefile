all: hello1 hello2

hello1: hello1.c
	${CC} -fopenmp hello1.c -o hello1

hello2: hello2.c
	${CC} -fopenmp hello2.c -o hello2

clean:
	rm -fv hello1 hello2
