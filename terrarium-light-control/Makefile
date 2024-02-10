

CCFLAGS=-g -Wall #-DTEST

CLIBS=wiringPi

all: terrarium PWM CalSunDay

%: %.c
	$(CC) $(CCFLAGS) $+ -o $@ -l$(CLIBS)

clean:
	$(RM) terrarium PWM calcOffset
