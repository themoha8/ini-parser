PROGRAM_NAME = example
OBJ_PATH = ./obj/
SRCMODULES = ini_parser.c
OBJMODULES = $(addprefix $(OBJ_PATH), $(SRCMODULES:.c=.o))
CC = gcc 

ifeq ($(RELEASE), 1)
	#CFLAGS = -Wall -Werror -Wextra -ansi -pedantic -O3
	CFLAGS = -Wall -Werror -Wextra -O3
else
	#CFLAGS = -Wall -Werror -Wextra -ansi -pedantic -g -O0
	CFLAGS = -Wall -Werror -Wextra -g -O0
endif

$(PROGRAM_NAME): example.c $(OBJMODULES)
	$(CC) $(CFLAGS) $^ -o $@

$(OBJ_PATH)%.o: %.c %.h
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_PATH)deps.mk: $(SRCMODULES)
	$(CC) -MM $^ > $@

.PHONY: clean
clean:
	rm -f $(OBJ_PATH)*.o $(PROGRAM_NAME) $(OBJ_PATH)deps.mk

ifneq (clean, $(MAKECMDGOALS))
-include $(OBJ_PATH)deps.mk
endif
