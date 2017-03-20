
LIB = libio.a

all:	$(LIB)


HEADERS = io.h port.h cpu.h realmodeint.h 8259PIC.h interrupts.h
OBJS = port.o cpu.o realmodeint.o 8259PIC.o interrupts.o

libio.a:	$(OBJS) $(HEADERS)
	ar rc $(LIB) $(OBJS)
	ranlib $(LIB)

%.o:	%.c $(HEADERS)
	gcc -m32 -fno-stack-protector -c $< -o $@

clean:
	rm $(OBJS) $(LIB)
