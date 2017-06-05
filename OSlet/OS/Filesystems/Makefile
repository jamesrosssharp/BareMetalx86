
LIB = libfs.a
all:	$(LIB)

HEADERS = fs.h fat.h
OBJS    = fs.o fat.o

$(LIB):	$(OBJS) $(HEADERS)
	ar rc $(LIB) $(OBJS)
	ranlib $(LIB)

%.o:	%.c $(HEADERS)
	gcc -m32 -fno-stack-protector -Werror  -c $< -o $@

clean:
	rm $(LIB)
	rm $(OBJS)
