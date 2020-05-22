all:
	 
	gcc -g -c main.c
	gcc -g -c functions.c
	gcc -g -o program.exe functions.c main.o -lm

# For some reason maybe the 'make run' command will not work. Please, run manually by 
# entering './program' command on your terminal
run:

	./program.exe

clean:
	rm *.exe
	rm *.o
	rm *.bmp
	rm *.bin