# Copyright PCLP Team, 2025

# Compiler setup.
CC=gcc
CFLAGS=-Wall -Wextra -std=c99

# Define targets.
TARGETS=runic

# Build all targets.
build: $(TARGETS)

runic: runic.c
	$(CC) $(CFLAGS) runic.c -o runic -lm

# Pack the solution into a zip file.
pack:
	zip -FSr 312CB_UdroiuDariuAdrian_Tema3.zip README Makefile *.c *.h 

# Clean the solution.
clean:
	rm -f $(TARGETS)

.PHONY: pack clean
