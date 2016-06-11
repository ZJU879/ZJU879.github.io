a.out : main.o ble.o http.o
	gcc -o a.out main.o ble.o http.o
main.o : main.c ble.h http.h flagdef.h
	gcc -c main.c
ble.o : ble.c ble.h
	gcc -c ble.c
http.o : http.c http.h
	gcc -c http.c
clean :
	rm *.o
