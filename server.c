#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "logUtil.h"

#define SERVER_PORT 8005       // Porta do servidor
#define MAX_MSG_SIZE 1024      // Tamanho máximo da mensagem

#define ERROR -1
#define SUCESS 1

#define SEQUENCE_LIMIT 100

int initServer (int *sock, struct sockaddr_in *serverAdress) {
    logInfo("Iniciando o server...");

    // Cria um socket UDP para o server
    *sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (*sock < 0) {
        logError("Falha ao criar o socket do server");
        return ERROR;
    }
    logInfo("Socket para server criado com sucesso");

    // Configura o endereço do server
    memset(serverAdress, 0, sizeof(*serverAdress));
    (*serverAdress).sin_family = AF_INET;
    (*serverAdress).sin_port = htons(SERVER_PORT);
    (*serverAdress).sin_addr.s_addr = htonl(INADDR_ANY);
    logInfo("Endereço do server configurado com sucesso");

    // Associa o socket ao endereço do servidor
    if (bind(*sock, (struct sockaddr*)serverAdress, sizeof(*serverAdress)) < 0) {
        logError("Falha ao associar o socket ao endereço do servidor");
        return ERROR;
    }
    logInfo("Socket associado ao endereço do servidor com sucesso");


    logInfo("Server iniciado na porta %d.", SERVER_PORT);
    return SUCESS;
}

int newSequence (int socketServer, struct sockaddr_in *clientAdress, socklen_t *clientAdressLen) {
    char msg[MAX_MSG_SIZE];
    int bytes_received;
    bytes_received = recvfrom(socketServer, msg, MAX_MSG_SIZE, 0, (struct sockaddr*)clientAdress, clientAdressLen);
    if (bytes_received < 0) {
        perror("Falha ao receber a mensagem");
        exit(1);
    }

    int sequence = atoi(msg);
    printf("Mensagem recebida de %s:%d: %d\n", inet_ntoa(clientAdress->sin_addr), ntohs(clientAdress->sin_port), sequence);
    
}

int main() {
    // Informações do cliente
    struct sockaddr_in clientAdress;
    socklen_t clientAdressLen = sizeof(clientAdress);

    // Informações do server
    int socketServer;
    struct sockaddr_in serverAdress;

    // Inicia server
    int result = initServer(&socketServer, &serverAdress);
    if(result == ERROR){
        logError("Não foi possível iniciar o servidor");
        exit(1);
    }

    // Recebe e processa as mensagens dos clientes
    char msg[MAX_MSG_SIZE];
    int bytes_received;
    while (1) {
        int new = newSequence(socketServer, &clientAdress, &clientAdressLen);
    }

    // Fecha o socket
    close(socketServer);

    return 0;
}
