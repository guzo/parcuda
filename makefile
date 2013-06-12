CXX = g++-4.7
CC  = $(CXX)
CXXFLAGS = -Wall -Wextra -std=c++11 -pedantic -Werror -I /usr/local/cuda-5.0/include/
LDLIBS = -lglut -lGLEW -lGL

run: main
	optirun ./$^

all: main

%.o : %.cu
	nvcc -c $^

main: main.o helper.o callbacks.o particles.o
	nvcc -o $@ $^ $(LDLIBS)

clean:
	-rm -v  *.o ./main

.PHONY: all run clean
