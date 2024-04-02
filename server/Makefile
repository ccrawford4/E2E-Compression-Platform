PROG = compdetect_server
OBJS = main.o socket.o ../shared/shared.o

CFLAGS = -g -I/Downloads/jansson-2.13/include -I../shared
LDFLAGS=-L/Downloads/jansson-2.13/lib -ljansson

CONFIG=config.json

%.o : %.c
	gcc $(CFLAGS) -c -o $@ $<

$(PROG) : $(OBJS)
	gcc $(CFLAGS) -o $@ $(OBJS) $(LDFLAGS)

clean :
	rm -rf $(PROG) $(OBJS) $(CONFIG)
