#CXXFLAGS=-std=c++11

# -g, makes sure debug symbols are included when building
build:
	g++ -g main.cpp input.cpp input.h -lncurses
