all: main.o job.o
	gcc main.o job.o -o hw1 -O3
main.o: main.c
	gcc -c main.c
job.o: job.c
	gcc -c job.c
clean:
	rm -rf hw1
	rm -rf *.o

