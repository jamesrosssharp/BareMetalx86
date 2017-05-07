
LIB = libklib.a

all:	$(LIB)


HEADERS = klib.h btree.h list.h math.h panic.h string.h
OBJS = btree.o list.o math.o panic.o string.o

$(LIB):	$(OBJS) $(HEADERS)
	ar rc $(LIB) $(OBJS)
	ranlib $(LIB)

%.o:	%.c $(HEADERS)
	gcc -m32 -fno-stack-protector -Werror -c $< -o $@

clean:
	rm $(OBJS) $(LIB)
