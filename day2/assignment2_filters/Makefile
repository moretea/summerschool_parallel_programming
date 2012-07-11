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
EXEC		= filter

VIEW		= display

RUN			= prun
RUNFLAGS	= -v -np 1
INIMAGE		= cat2.ppm
OUTIMAGE	= cat2out.ppm
RUNARGS		= -i $(INIMAGE) -o $(OUTIMAGE)

CFILES 		= main.c filters.c ppm.c timer.c
OFILES		= $(CFILES:%.c=%.o)

all:	$(OFILES)
		$(CC) $(LIBDIR) $(LIB) $(CFLAGS) -o $(EXEC) $(OFILES) 

run:
		$(RUN) $(RUNFLAGS) $(EXEC) $(RUNARGS) -f $(filter)
		$(VIEW) $(OUTIMAGE)

rund:
		./$(EXEC) $(RUNARGS) -f $(filter)
		$(VIEW) $(OUTIMAGE)

clean:
		rm $(EXEC) *.o
