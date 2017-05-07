
LIB = libgfx.a
all:	$(LIB)


HEADERS = gfx.h vesa.h fb.h primitives.h
OBJS = vesa.o fb.o primitives.o

$(LIB):	$(OBJS) $(HEADERS)
	ar rc $(LIB) $(OBJS)
	ranlib $(LIB)

%.o:	%.c $(HEADERS)
	gcc -m32 -fno-stack-protector -Werror  -c $< -o $@


clean:
	rm $(LIB)
	rm $(OBJS)
