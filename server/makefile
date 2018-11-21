CC=g++-8
CFLAGS=-Ofast -march=native $(shell pkg-config --cflags --libs opencv) -lboost_system -lboost_thread-mt -lboost_chrono-mt -lssl -lcrypto -I/usr/local/opt/openssl/include -L/usr/local/opt/openssl/lib
BDIR=build
SDIR=src

$(BDIR)/main.out: src/*.cpp
	mkdir $(BDIR)
	$(CC) -o $(BDIR)/main.out src/*.cpp $(CFLAGS)

$(BDIR)/vector3_test.out: src/vector3.cpp tests/vector3_test.cpp
	mkdir $(BDIR)
	$(CC) -o $(BDIR)/vector3_test src/vector3.cpp tests/vector3_test.cpp
