CFLAGS = -Wall -g
 
objs = client.o escreva.o
 
# regra default (primeira regra)
all: hello
 
# regras de ligacao
hello: $(objs)
 
# regras de compilação
hello.o:   hello.c escreva.h
escreva.o: escreva.c escreva.h
 
# remove arquivos temporários
clean:
	-rm -f $(objs) *~
 
# remove tudo o que não for o código-fonte
purge: clean
	-rm -f hello
