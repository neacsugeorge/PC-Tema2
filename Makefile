GCC=gcc
FLAGS=-Wall

build: server client

server: server.o
	$(GCC) -o $@ $^

server.o: server.c
	$(GCC) $(FLAGS) -c $^

client: client.o
	$(GCC) -o $@ $^

client.o: client.c
	$(GCC) $(FLAGS) -c $^