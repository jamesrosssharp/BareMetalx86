
LIB = libblock.a
all:	$(LIB)

HEADERS = block.h biosblock.h mbrpartition.h partition.h
OBJS = block.o biosblock.o mbrpartition.o partition.o

$(LIB):	$(OBJS) $(HEADERS)
	ar rc $(LIB) $(OBJS)
	ranlib $(LIB)

%.o:	%.c $(HEADERS)
	gcc -m32 -fno-stack-protector -Werror  -c $< -o $@

clean:
	rm $(LIB)
	rm $(OBJS)
