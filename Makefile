readSDS011: main.o serial.o
	gcc -o readSDS011 *.o

main.o: main.c
	gcc -c main.c

serial.o: serial.c
	gcc -c serial.c

clean:
	rm -f readSDS011 *.o
