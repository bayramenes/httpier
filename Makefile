# file to compile the project

# Compiler

CC = gcc

# Flags
CFLAGS=  -Wno-pointer-sign

app/server.o: app/server.c
	@echo "Compiling server.c"
	@$(CC) $(CFLAGS) -c app/server.c -o app/server.o

app/helpers.o: app/helpers.c
	@echo "Compiling helpers.c"
	@$(CC) $(CFLAGS) -c app/helpers.c -o app/helpers.o

app/parsers.o: app/parsers.c
	@echo "Compiling parsers.c"
	@$(CC) $(CFLAGS) -c app/parsers.c -o app/parsers.o

app/hashtable.o: app/hashtable.c
	@echo "Compiling hashtable.c"
	@$(CC) $(CFLAGS) -c app/hashtable.c -o app/hashtable.o

final: app/server.o app/helpers.o app/parsers.o app/hashtable.o
	@echo "Linking all files"
	@$(CC) $(CFLAGS) app/server.o app/helpers.o app/parsers.o app/hashtable.o -o httpier

all: final


clean:
	@echo "Cleaning up"
	@rm -f app/*.o final