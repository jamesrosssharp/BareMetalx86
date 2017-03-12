
LIB = libconsole.a

all:	$(LIB)


HEADERS = console.h vgaconsole.h kprintf.h
OBJS = console.o vgaconsole.o kprintf.o

$(LIB):	$(OBJS) $(HEADERS)
	ar rc $(LIB) $(OBJS)
	ranlib $(LIB)

%.o:	%.c $(HEADERS)
	gcc -m32 -fno-stack-protector -c $< -o $@

clean:
	rm $(OJBS) $(LIB)
