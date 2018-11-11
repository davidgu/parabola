CC=g++
CFLAGS=-std=c++11 -Ofast -march=native $(pkg-config --cflags --libs opencv)
BDIR=build

$(BDIR)/vector3_test.out: src/vector3.cpp tests/vector3_test.cpp
	$(CC) -o $(BDIR)/vector3_test src/vector3.cpp tests/vector3_test.cpp
