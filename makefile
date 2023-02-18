CFLAGS = -Wall -g
CC=gcc

# regra default (primeira regra)
all: client server
 
# regras de ligacao
client: client.c
	$(CC) -o $@ $^ $(DEBUG_FLAGS) -lstdc++fs

server: server.c
	$(CC) -o $@ $^ $(DEBUG_FLAGS) -lstdc++fs

# remove arquivos temporários
clean:
	-rm -f $(objs) *~
 
# remove tudo o que não for o código-fonte
purge: clean
	-rm -f hello
