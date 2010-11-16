SRCMODULES = buffer.c serverinfo.c client.c 
OBJMODULES = $(SRCMODULES:.c=.o)
CFLAGS = -g -Wall 

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $< -o $@

c-gameserv: main.c $(OBJMODULES)
	$(CC) $(CFLAGS) $^ -o $@

ifneq (clean, $(MAKECMDGOALS))
-include deps.mk
endif

deps.mk: $(SRCMODULES)
	$(CC) -MM $^ > $@

clean:
	rm -f *.o c-gameserv deps.mk *~
