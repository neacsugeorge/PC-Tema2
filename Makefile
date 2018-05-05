GCC=gcc
FLAGS=-Wall

build: server client

server: server.o src/CardManager.o src/Logger.o src/ConnectionManager.o src/CommandManager.o
	$(GCC) -o $@ $^

server.o: server.c
	$(GCC) $(FLAGS) -c $^

src/CardManager.o: src/CardManager.c src/CardManager.h
	$(GCC) $(FLAGS) -c $< -o $@

src/Logger.o: src/Logger.c src/Logger.h
	$(GCC) $(FLAGS) -c $< -o $@

src/ConnectionManager.o: src/ConnectionManager.c src/ConnectionManager.h
	$(GCC) $(FLAGS) -c $< -o $@

src/CommandManager.o: src/CommandManager.c src/CommandManager.h
	$(GCC) $(FLAGS) -c $< -o $@

client: client.o src/CardManager.o src/Logger.o src/ConnectionManager.o src/CommandManager.o
	$(GCC) -o $@ $^

client.o: client.c
	$(GCC) $(FLAGS) -c $^