################################################################
#                                                              #
################################################################

#If you want a colored Output for 
#COLOROUT=-D__coloredout__

#If you need the Debuging Symbols
#DEBUG=-ggdb

#specify a path where we can find the .so files
LIBRARY_PATH=/usr/lib/
CFLAG=-Wall $(DEBUG) $(COLOROUT)
BIN=bin

all: prepair temper.o
	$(CC) -L $(LIBRARY_PATH):. -lusb temper.o main.o -o bin/temper -O2

temper.o:
	$(CC) $(CFLAG) -c temper.c
	$(CC) $(CFLAG) -c main.c

testing:
	$(CC) $(CFLAG) -o testing -lusb testing.c

prepair:
	mkdir -p bin

debug: temper.o
	$(CC) $(CFLAG) -ggdb -L. -lusb temper.o main.o -o bin/temper

install: rules-install
	cp bin/temper /usr/lib/nagios/plugins/check_temper

rules-install:
	cp common/99-tempsensor.rules /etc/udev/rules.d/

clean:
	rm *.o
	rm -Rfv bin/temper
