NAME=ifj19comp
SRCFOLDER := .
OBJFOLDER := obj
SRCFILES := $(wildcard $(SRCFOLDER)/*.c)
OBJFILES := $(patsubst %.c,$(OBJFOLDER)/%.o,$(notdir $(SRCFILES)))
CC=gcc
CFLAGS= -std=c99 -pedantic -Wall -Wextra -g

all: $(NAME)

$(OBJFOLDER)/%.o : $(SRCFOLDER)/%.c
	mkdir -p $(OBJFOLDER)
	$(CC) $(CFLAGS) -c -o $@ $<


clean:
	rm -rf $(OBJFOLDER)/
	rm -f $(NAME)

test:
	./$(NAME) vita

dep:
	$(CC) -MM $(SRCFOLDER)/*.c >dep.list

-include dep.list

$(NAME): $(OBJFILES)
	$(CC) $(CFLAGS) $(OBJFILES) -o $@
