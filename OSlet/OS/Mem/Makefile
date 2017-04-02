
LIB = libmem.a

all:	$(LIB)


HEADERS = mem.h freelist.h
OBJS = mem.o freelist.o

$(LIB):	$(OBJS) $(HEADERS)
	ar rc $(LIB) $(OBJS)
	ranlib $(LIB)

%.o:	%.c $(HEADERS)
	gcc -m32 -fno-stack-protector -c $< -o $@

clean:
	rm $(OBJS) $(LIB)
