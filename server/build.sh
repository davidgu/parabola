clang++ -std=c++11 -Ofast -march=native $(pkg-config --cflags --libs opencv) *.cpp -o main.o
