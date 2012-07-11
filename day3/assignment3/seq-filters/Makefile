CC		= g++
CC_FLAGS	= -O3 -m64 -Wall

CC_SOURCES	= bmp_io.cc timer.cc filters.cc

CC_OBJECTS	= $(CC_SOURCES:%.cc=%.o)

%.o:		%.cc
		$(CC) $(CC_FLAGS) -c $< -o $@

all:		filters compare_images

filters:	$(CC_OBJECTS)
		$(CC) $(CC_FLAGS) $^ -o $@

compare_images: compare_images.cc bmp_io.cc
		$(CC) $(CC_FLAGS) $^ -o $@

clean:
		rm -f *.o filters compare_images *~

run: all
	./filters ../input-images/image2.bmp
