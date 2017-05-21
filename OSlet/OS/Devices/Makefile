
all:	libdevices.a

LIBS =  Block/libblock.a 

libdevices.a:	$(LIBS)
	@printf "CREATE libdevices.a\n \
    		ADDLIB Block/libblock.a\n	\
		SAVE\n		\
    		END" | ar -M
	ranlib libdevices.a


.PHONY:	$(LIBS)

Block/libblock.a:
	make -C Block

clean:
	rm *.a *.o
	make -C Block clean
