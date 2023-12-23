CFLAGS = -Wall -Werror -g3

clean:
	rm -f *.out *.o
	ls | grep -v "\." | grep -v "Makefile" | xargs -r rm -f
	clear

run%: clean %
	echo "Compilation finished"
	./$*

%.o: %.c

%: %.o

