all:	mathtest


mathtest:	main.o math.o 
	gcc main.o math.o -o mathtest -lstdc++

main.o:	main.cpp
	gcc main.cpp -std=c++11 -c -DUNIT_TEST 

math.o: ../../../OS/Lib/math.c
	gcc ../../../OS/Lib/math.c -c -DUNIT_TEST

clean:
	rm *.o btreetest 
