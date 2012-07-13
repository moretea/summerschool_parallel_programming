# Summerschool Getting Moore from Multicores
#
# Makefile for assignment 3
#
# (c) 30-06-2011 Roy Bakker - University of Amsterdam
#

CC			= gcc -std=c99 -fopenmp
CFLAGS		= -Wall -O3 -W -Wno-unused
LIBDIR		=
LIB			= 
EXEC		= mandel

VIEW		= display

RUN			= prun
RUNFLAGS	= -v -np 1
OUTIMAGE	= mandel.ppm

CFILES 		= mandel.c
OFILES		= $(CFILES:%.c=%.o)

all:		$(EXEC)

$(EXEC):	$(OFILES)
			$(CC) $(LIBDIR) $(LIB) $(CFLAGS) -o $(EXEC) $(OFILES) 

run:		$(EXEC)
			$(RUN) $(RUNFLAGS) $(EXEC) > $(OUTIMAGE)
			$(VIEW) $(OUTIMAGE)

rund:		$(EXEC)
			./$(EXEC) > $(OUTIMAGE)
			$(VIEW) $(OUTIMAGE)

clean:
			rm $(EXEC) *.o
