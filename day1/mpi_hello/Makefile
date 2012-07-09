all: hello invoke

hello: 
	module load openmpi/gcc && \
	mpicc -o hello hello.c

invoke:
	module load prun && \
	prun -v -8 -np 2 -sge-script $$PRUN_ETC/prun-openmpi `pwd`/hello

