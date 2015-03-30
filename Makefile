ddt: main.o keyboard.o internode.o functions.o aux1.o
	gcc -o ddt main.o keyboard.o internode.o functions.o aux1.o -g
	
main.o: main.c keyboard.h internode.h
	gcc -c main.c -g
	
keyboard.o: keyboard.c keyboard.h	
	gcc -c keyboard.c -g

internode.o: internode.c internode.h	
	gcc -c internode.c -g
	
functions.o: functions.c functions.h
	gcc -c functions.c -g
	
aux1.o: aux1.c aux1.h
	gcc -c aux1.c -g
	
clean:
	rm *.o
