MAKE = g++
CFLAGS = -Wall -g -O1
TARGET = littleHttpd
OBJ = $(patsubst %.c, %.o, $(wildcard *.c))

%.o:%.c
	$(MAKE) $(CFLAGS) -c $< -o $@

all:$(OBJ)
	$(MAKE) $(CFLAGS) -o $(TARGET) $+
	@-rm *.o -rvf

.PHONY:clean
clean:
	-rm *.o $(TARGET) -rvf 
