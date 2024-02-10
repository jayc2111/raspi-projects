

CCFLAGS=-g -Wall #-DTEST

CLIBS=wiringPi

all:read timer

%: %.c
	$(CC) $(CCFLAGS) $+ -o $@ -l$(CLIBS)

read: read.c
	$(CC) $(CCFLAGS) $+ -o $@ -l$(CLIBS) -lrt

clean:
	$(RM) read
