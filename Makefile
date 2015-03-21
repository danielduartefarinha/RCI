ddt: ddt.o aux.o
	gcc -o ddt ddt.o aux.o -g
	
ddt.o: ddt.c aux.h
	gcc -c ddt.c -g
	
aux.o: aux.c aux.h
	gcc -c aux.c -g
	
clean:
	rm *.o
