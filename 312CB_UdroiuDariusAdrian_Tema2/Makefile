# Copyright PCLP Team, 2025

# Compiler setup.
CC=gcc
CFLAGS=-Wall -Wextra -std=c99

# Define targets.
TARGETS=battleships

# Build all targets.
build: $(TARGETS)

battleships: battleships.c
	$(CC) $(CFLAGS) battleships.c -o battleships

# Pack the solution into a zip file.
pack:
	zip -FSr 312CB_UdroiuDariuAdrian_Tema2.zip README Makefile *.c *.h 

# Clean the solution.
clean:
	rm -f $(TARGETS)

.PHONY: pack clean
