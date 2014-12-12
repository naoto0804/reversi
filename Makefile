CC = gcc
CFLAGS = -Wall -g
OBJS = main5.o move1.o

a.out:	$(OBJS)
	$(CC) $(OBJS) $(LDLIBS)

clean:
	rm -f *~ *.o a.out