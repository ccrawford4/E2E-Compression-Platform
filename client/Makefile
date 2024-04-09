PROG = compdetect_client
OBJS = main.o socket.o ../shared/shared.o ../shared/json.o

CFLAGS = -g -I../jansson-2.13/include -I../shared
LDFLAGS = -L/Downloads/jansson-2.13/lib -ljansson
RESULT = result.txt

%.o : %.c
	gcc $(CFLAGS) -c -o $@ $<

$(PROG) : $(OBJS)
	gcc $(CFLAGS) -o $@ $(OBJS) $(LDFLAGS)

clean :
	rm -rf $(PROG) $(OBJS) $(RESULT)
