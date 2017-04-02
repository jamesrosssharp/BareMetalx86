all:	freelisttest


freelisttest:	main.o	freelist.o 
	gcc main.o freelist.o -o freelisttest -lstdc++

main.o:	main.cpp
	gcc main.cpp -std=c++11 -c 

freelist.o: ../../../OS/Mem/freelist.c
	gcc ../../../OS/Mem/freelist.c -c -DUNIT_TEST

clean:
	rm *.o freelisttest 
