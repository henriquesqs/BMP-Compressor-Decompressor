#all:
	
#	gcc -g -c compressor.c
#	gcc -g -c functions.c
#	gcc -g -o compressor functions.c compressor.o -lm

all:
	 
	gcc -g -c main.c
	gcc -g -c functions.c
	gcc -g -o program functions.c main.o -lm

# For some reason maybe the 'make run' command will not work. Please, run manually by 
# entering './compressor' command on your terminal
run:

	./compressor

clean:
	rm *.o
	rm *.exe

teste:
	
	gcc -g -c testeDCT.c
	gcc -g -c functions.c
	gcc -g -o teste.exe functions.c testeDCT.o -lm