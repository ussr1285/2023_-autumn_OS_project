# Compiler and flags
CC = gcc

# Targets
all: server client

# Server compilation
server: factoryManageServer.o makeCanFood.o
	$(CC) factoryManageServer.o makeCanFood.o -o server

factoryManageServer.o: factoryManageServer.c
	$(CC) -c factoryManageServer.c

makeCanFood.o: makeCanFood.c
	$(CC) -c makeCanFood.c

# Client compilation
client: client.o ft_itoa.o ft_strdup.o
	$(CC) client.o ft_itoa.o ft_strdup.o -o client

client.o: client.c
	$(CC) -c client.c

ft_itoa.o: ft_itoa.c
	$(CC) -c ft_itoa.c

ft_strdup.o: ft_strdup.c
	$(CC) -c ft_strdup.c

# Clean
clean:
	rm -f *.o server client

# Phony targets
.PHONY: all clean
