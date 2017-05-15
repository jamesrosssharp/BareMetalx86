
LIB = libmem.a

all:	$(LIB)


HEADERS = mem.h freelist.h buddy.h kr.h
OBJS = mem.o freelist.o buddy.o kr.o

$(LIB):	$(OBJS) $(HEADERS)
	ar rc $(LIB) $(OBJS)
	ranlib $(LIB)

%.o:	%.c $(HEADERS)
	gcc -m32 -fno-stack-protector -Werror -c $< -o $@

clean:
	rm $(OBJS) $(LIB)
