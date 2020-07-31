all:
	 
	gcc -Wall -c main.c -g
	gcc -Wall -c functions.c -g
	gcc -Wall -o program.exe functions.c main.o -lm -g

# For some reason maybe the 'make run' command will not work. Please, run manually by 
# entering './program.exe' command on your terminal
run:

	./program.exe

clean:
	
	rm *.o
	rm *.bin
	rm *.bmp
	rm *.exe

valgrind: 
	valgrind --log-file="valgrindOutput.txt" --leak-check=full ./program