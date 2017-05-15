all:	krtest


krtest:	main.o	kr.o 
	gcc main.o kr.o -o krtest -lstdc++

main.o:	main.cpp
	gcc main.cpp -std=c++11 -c -DUNIT_TEST 

kr.o: ../../../OS/Mem/kr.c
	gcc ../../../OS/Mem/kr.c -c -DUNIT_TEST


clean:
	rm *.o buddytest
