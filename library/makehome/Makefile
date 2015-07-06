CC	=	gcc

OBJ	=	$(SRC:.c=.o)

SNAME	=	libmy_cometd_`uname -m`-`uname -s`.a

FLAGS	=	 -Wall -Werror -arch x86_64 -g3 -I../includes  -I/usr/local/Cellar/jansson/2.7/include -I/usr/local/lib -lcurl -I/usr/local/include -I/opt/X11/include/GL

SRC	=	$(wildcard ../*.c)

SRPING = $(filter-out ../pongtest.c, $(wildcard ../*.c))

SRPONG = $(filter-out ../ztest.c, $(wildcard ../*.c))

CFLAGS	=	`pkg-config --cflags --libs jansson` `pkg-config --cflags --libs nopoll`


all:
	gcc -o CometD $(SRC) $(FLAGS) $(CFLAGS)

static:
	$(OBJ)
	$(CC) -c $(SRC) $(FLAGS)
	ar rc $(SNAME) $(OBJ)
	ranlib $(SNAME)

ping:
	$(CC) -o pinger $(SRPING) $(CFLAGS)

pong:
	$(CC) -o ponger $(SRPONG) $(CFLAGS)
