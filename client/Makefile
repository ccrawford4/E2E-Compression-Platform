PROG = compdetect_client
OBJS = main.o socket.o parser.o

CFLAGS = -g -I/Downloads/jansson-2.13/include
LDFLAGS=-L/Downloads/jansson-2.13/lib -ljansson

%.o : %.c
	gcc $(CFLAGS) -c -o $@ $<

$(PROG) : main.h $(OBJS)
	gcc $(CFLAGS) -o $@ $(OBJS) $(LDFLAGS)

clean :
	rm -rf $(PROG) $(OBJS)
