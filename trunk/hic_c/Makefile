BIN=bin
INCLUDE=/usr/include/
CFLAG=-Wall -ggdb

all: prepair temper.o
	$(CC) -L $(INCLUDE) -lusb temper.o main.o -o bin/temper -L.

temper.o:
	$(CC) $(CFLAG) -c temper.c
	$(CC) $(CFLAG) -c main.c

testing:
	$(CC) $(CFLAG) -o testing -lusb testing.c

prepair:
	mkdir -p bin

debug: temper.o
	$(CC) $(CFLAG) -ggdb -L. -lusb temper.o main.o -o bin/temper

install:
	cp bin/temper /usr/lib/nagios/plugins/check_temper

clean:
	rm *.o
	rm -Rfv bin/temper
