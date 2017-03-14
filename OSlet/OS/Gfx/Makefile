
LIB = libgfx.a
all:	$(LIB)


HEADERS = gfx.h vesa.h
OBJS = vesa.o

$(LIB):	$(OBJS) $(HEADERS)
	ar rc $(LIB) $(OBJS)
	ranlib $(LIB)

%.o:	%.c $(HEADERS)
	gcc -m32 -fno-stack-protector -c $< -o $@


clean:
	rm $(LIB)
	rm $(OBJS)
