
LIB = libbios.a
all:	$(LIB)


HEADERS = bda.h bios.h biosmem.h
OBJS = bda.o biosmem.o

libbios.a:	$(OBJS) $(HEADERS)
	ar rc $(LIB) $(OBJS)
	ranlib $(LIB)

%.o:	%.c $(HEADERS)
	gcc -m32 -fno-stack-protector -Werror  -c $< -o $@


clean:
	rm $(LIB)
	rm $(OBJS)
