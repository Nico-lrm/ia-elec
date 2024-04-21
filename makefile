DEBUG=no
CC=g++

ifeq ($(DEBUG), yes)
	CFLAGS=-std=c++20 -Wall -W -pthread -fstack-protector-all
else
	CFLAGS=-std=c++20 -O2 -pthread -fstack-protector-all
endif

LDFLAGS=-lpthread

.PHONY: clean

# Standard
bin/app: obj/main.o
	$(CC) $^ -o $@ $(LDFLAGS)

obj/main.o: src/main.cpp src/Reseau.hpp src/Couche.hpp src/Neurone.hpp vendor/csv.h
	$(CC) $(CFLAGS) -c $<

# Clean
clean:
	rm -r obj/*.o bin/app.out