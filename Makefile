.PHONY: all clean
CFLAGS = -Wall -Werror -g3
LDFLAGS = -g3

clean:
	rm -f *.out *.o
	ls | grep -v "\." | grep -v "Makefile" | xargs -r rm -f
	clear

one: main.o 1.o
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ main.o 1.o

