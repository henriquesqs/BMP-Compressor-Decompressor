all:
	 
	gcc -Wall -c main.c -g
	gcc -Wall -c functions.c -g
	gcc -Wall -o program functions.c main.o -lm -g

# For some reason maybe the 'make run' command will not work. Please, run manually by 
# entering './program' command on your terminal
run:

	./program

clean:
	rm *.o
	rm *.bin
	rm *.bmp

valgrind: 
	valgrind --log-file="valgrindOutput.txt" --leak-check=full ./program