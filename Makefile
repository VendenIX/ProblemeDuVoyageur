#!/bin/bash

all:
	@echo "Compiling..."
	g++ ex2.cpp -llpsolve55 -lcolamd -ldl -o main.o
	@echo "Compiling done"
	./main.o
