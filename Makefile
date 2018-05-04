GCC=gcc
FLAGS=-Wall

build: server client

server: server.o src/CardManager.o src/Logger.o
	$(GCC) -o $@ $^

server.o: server.c
	$(GCC) $(FLAGS) -c $^

src/CardManager.o: src/CardManager.c src/CardManager.h
	$(GCC) $(FLAGS) -c $< -o $@

src/Logger.o: src/Logger.c src/Logger.h
	$(GCC) $(FLAGS) -c $< -o $@

client: client.o
	$(GCC) -o $@ $^

client.o: client.c
	$(GCC) $(FLAGS) -c $^