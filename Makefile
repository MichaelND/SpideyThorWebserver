CC=		gcc
CFLAGS=		-g -gdwarf-2 -Wall -std=gnu99
LD=		gcc
LDFLAGS=	-L.
TARGETS=	spidey

all:		$(TARGETS)

forking.o: forking.c
	$(CC) $(CFLAGS) -c -o $@ $^

handler.o: handler.c
	$(CC) $(CFLAGS) -c -o $@ $^

request.o: request.c
	$(CC) $(CFLAGS) -c -o $@ $^

single.o: single.c
	$(CC) $(CFLAGS) -c -o $@ $^

socket.o: socket.c
	$(CC) $(CFLAGS) -c -o $@ $^

spidey.o: spidey.c
	$(CC) $(CFLAGS) -c -o $@ $^

utils.o: utils.c
	$(CC) $(CFLAGS) -c -o $@ $^

spidey: forking.o handler.o request.o single.o socket.o spidey.o utils.o
	$(LD) $(LDFLAGS) -o $@ $^

clean:
	@echo Cleaning...
	@rm -f $(TARGETS) *.o *.log *.input

.PHONY:		all clean
