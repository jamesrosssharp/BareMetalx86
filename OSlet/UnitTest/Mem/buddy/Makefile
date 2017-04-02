all:	buddytest


buddytest:	main.o	buddy.o freelist.o btree.o math.o 
	gcc main.o freelist.o btree.o buddy.o math.o -o buddytest -lstdc++

main.o:	main.cpp
	gcc main.cpp -std=c++11 -c -DUNIT_TEST 

buddy.o: ../../../OS/Mem/buddy.c
	gcc ../../../OS/Mem/buddy.c -c -DUNIT_TEST

freelist.o: ../../../OS/Mem/freelist.c
	gcc ../../../OS/Mem/freelist.c -c -DUNIT_TEST

btree.o: ../../../OS/Lib/btree.c
	gcc ../../../OS/Lib/btree.c -c -DUNIT_TEST

math.o: ../../../OS/Lib/math.c
	gcc ../../../OS/Lib/math.c -c -DUNIT_TEST

clean:
	rm *.o buddytest
