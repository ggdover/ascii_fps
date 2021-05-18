#CXXFLAGS=-std=c++11

# -g, makes sure debug symbols are included when building
build:
	g++ main.cpp input.cpp input.h rendering.cpp rendering.h globals.h -lncurses
