all:
	 
	gcc -g -c main.c
	gcc -g -c functions.c
	gcc -g -o program functions.c main.o -lm

# For some reason maybe the 'make run' command will not work. Please, run manually by 
# entering './program' command on your terminal
run:

	./program

clean:
	rm *.o
	rm *.exe