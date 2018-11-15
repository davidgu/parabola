CC=g++
CFLAGS=-std=c++11 -Ofast -march=native $(shell pkg-config --cflags --libs opencv)
BDIR=build
SDIR=src

$(BDIR)/main.out: src/*.cpp
	mkdir $(BDIR)
	$(CC) -o $(BDIR)/main.out src/*.cpp $(CFLAGS)

$(BDIR)/vector3_test.out: src/vector3.cpp tests/vector3_test.cpp
	mkdir $(BDIR)
	$(CC) -o $(BDIR)/vector3_test src/vector3.cpp tests/vector3_test.cpp
