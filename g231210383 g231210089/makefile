CC      = gcc
CFLAGS  = -Wall -Wextra -std=c99
TARGET  = tarsau
SRCS    = tarsau.c archive.c extract.c
OBJS    = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c tarsau.h
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(OBJS) $(TARGET)