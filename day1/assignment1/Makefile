# Summerschool Getting Moore from Multicores
#
# Makefile for assignment 1
#
# (c) 30-06-2011 Roy Bakker - University of Amsterdam
#

CC              = gcc
MPICC           = mpicc
CFLAGS		= -Wall -O3 -W -Wno-unused -std=c99

LIBDIR		=
LIB		= -lm 
EXEC		= filter
DIST            = assignment1

VIEW		= display

image           = cat2
filter          = invert
iters           = 1
nodes           = 1
ppn             = 1

RUN		= prun
RUNMPI          = $(RUN) -sge-script $$PRUN_ETC/prun-openmpi
RUNFLAGS	= -v -np $(nodes) -$(ppn)

INIMAGE		= $(image).ppm
OUTSUFFIX       = out
OUTIMAGE	= $(image)-$(filter)-$(OUTSUFFIX).ppm
RUNARGS		= -i $(INIMAGE) -o $(OUTIMAGE) -f $(filter) -c $(iters)

CFILES 		= main.c filters.c ppm.c timer.c
OFILES		= $(CFILES:%.c=%.o)

all:		clean $(OFILES)
		$(CC) $(LIBDIR) $(LIB) $(CFLAGS) -o $(EXEC) $(OFILES) 

mpi:		CC = $(MPICC)
mpi:		CFLAGS += -DMPI
mpi:		clean $(OFILES) 
		$(MPICC) $(LIBDIR) $(LIB) $(CFLAGS) -o $(EXEC) $(OFILES) 

run:
		$(RUN) $(RUNFLAGS) $(EXEC) $(RUNARGS)
		$(VIEW) $(OUTIMAGE)

runmpi:
		$(RUNMPI) $(RUNFLAGS) $(EXEC) $(RUNARGS)
		$(VIEW) $(OUTIMAGE)

runlocal:
		./$(EXEC) $(RUNARGS)
		$(VIEW) $(OUTIMAGE)

mpilocal:
		mpirun -np $(ppn) $(EXEC) $(RUNARGS)
		$(VIEW) $(OUTIMAGE)

clean:
		rm -f $(EXEC) *.o *.tgz

dist:           clean
		rm -f *$(OUTSUFFIX).ppm
		mkdir $(DIST)	
		cp -t $(DIST) *.c *.h *.ppm Makefile
		tar czvf $(DIST).tgz $(DIST)/
		rm -rf $(DIST)

