SHELL = /bin/sh
FLAGS = -Wall -g
CC = g++

all:    2pipe2child 2pipe3child dynapipe
	
2pipe2child:     TwoPipesTwoChildren.cpp
	$(CC) $(FLAGS) -o $@ TwoPipesTwoChildren.cpp
	
2pipe3child:     TwoPipesThreeChildren.cpp
	$(CC) $(FLAGS) -o $@ TwoPipesThreeChildren.cpp
	
dynapipe:     dynapipe.cpp
	$(CC) $(FLAGS) -o $@ dynapipe.cpp
	
clean:
	rm 2pipe2child 2pipe3child dynapipe