
LIB = libio.a

all:	$(LIB)


HEADERS = io.h port.h
OBJS = port.o

libio.a:	$(OBJS) $(HEADERS)
	ar rc $(LIB) $(OBJS)
	ranlib $(LIB)

%.o:	%.c $(HEADERS)
	gcc -m32 -fno-stack-protector -c $< -o $@

clean:
	rm $(OBJS) $(LIB)
