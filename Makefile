ddt: main.o keyboard.o internode.o functions.o aux.o
	gcc -o ddt main.o keyboard.o internode.o functions.o aux.o -g
	
main.o: main.c keyboard.h internode.h
	gcc -c main.c -g
	
keyboard.o: keyboard.c keyboard.h	
	gcc -c keyboard.c -g

internode.o: internode.c internode.h	
	gcc -c internode.c -g
	
functions.o: functions.c functions.h
	gcc -c functions.c -g
	
aux.o: aux.c aux.h
	gcc -c aux.c -g
	
clean:
	rm *.o
