CFLAGS = -Wall -g
CC=gcc

# regra default (primeira regra)
all: client server
 
# regras de ligacao
client: client.c logUtil.o
	$(CC) -o $@ $^ $(DEBUG_FLAGS) -lstdc++fs

server: server.c logUtil.o
	$(CC) -o $@ $^ $(DEBUG_FLAGS) -lstdc++fs

logUtil.o: logUtil.c logUtil.h
	$(CC) -c $^ $(DEBUG_FLAGS) -lstdc++fs

# remove arquivos temporários
clean:
	-rm -f *.o *.h.gch *~
 
# remove tudo o que não for o código-fonte
purge: clean
	-rm -f client server
