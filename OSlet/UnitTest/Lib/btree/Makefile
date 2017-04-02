all:	btreetest


btreetest:	main.o	freelist.o btree.o 
	gcc main.o freelist.o btree.o -o btreetest -lstdc++

main.o:	main.cpp
	gcc main.cpp -std=c++11 -c -DUNIT_TEST 

freelist.o: ../../../OS/Mem/freelist.c
	gcc ../../../OS/Mem/freelist.c -c -DUNIT_TEST

btree.o: ../../../OS/Lib/btree.c
	gcc ../../../OS/Lib/btree.c -c -DUNIT_TEST

clean:
	rm *.o btreetest 
