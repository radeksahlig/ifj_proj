CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -Werror

EXECUTABLE = ifj19comp
SRCS = *.c
OBJS = $(shell $(CC) $(CFLAGS) -MM $(SRCS) | grep ':' | cut -d ':' -f1 | tr '\n' ' ')

$(EXECUTABLE): $(OBJS)
