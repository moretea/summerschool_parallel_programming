NVCC		= nvcc
CC		= g++
#CU_FLAGS	= -O3 -g -deviceemu
CU_FLAGS	= -O3 -g --ptxas-options=-v -arch compute_20 -code sm_20
CC_FLAGS	= -O3 -m64 -Wall

CU_SOURCES	= cuda-filters.cu
CC_SOURCES	= bmp_io.cc timer.cc

CU_OBJECTS	= $(CU_SOURCES:%.cu=%.o)
CU_PTX		= $(CU_SOURCES:%.cu=%.ptx)
CC_OBJECTS	= $(CC_SOURCES:%.cc=%.o)

%.o:		%.cu
		$(NVCC) $(CU_FLAGS) -c $< -o $@

%.o:		%.cc
		$(CC) $(CC_FLAGS) -c $< -o $@

%.ptx:		%.cu
		$(NVCC) $(CU_FLAGS) --ptx $< -o $@

all:		cuda-filters compare_images

cuda-filters:	$(CU_OBJECTS) $(CC_OBJECTS)
		$(NVCC) $^ -o $@

compare_images: compare_images.cc bmp_io.cc
		$(CC) $(CC_FLAGS) $^ -o $@

ptx:		$(CU_PTX) 

clean:
		rm -f *.o cuda-filters.ptx cuda-filters compare_images *~

run: all
	prun -v -np 1 -native '-l gpu=GTX480' cuda-filters ../input-images/downtown_bflo2.bmp

test: all
	compare_images
	filter ../
