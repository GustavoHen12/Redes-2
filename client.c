/**
 * @file client.c
 * @author Gustavo Henrique da Silva Barbosa (ghsb19) e Calebe Pompeo Helpa(cph19)
 * @brief Cliente do canhão UDP. Envia diversas mensagens sequências para o servidor que irá calcular
 * a taxa de erro 
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdarg.h>

#include "logUtil.h"

#define SERVER_IP "192.168.1.117"  // Endereço IP do servidor
#define SERVER_PORT 8005           // Porta do servidor
#define MAX_MSG_SIZE 1024          // Tamanho máximo da mensagem

#define ERROR -1
#define SUCESS 1

#define SEQUENCE_LIMIT 10

int initClient (int *sock, struct sockaddr_in *serverAdress) {
    logInfo("Iniciando o cliente...");

    // Cria um socket UDP para o cliente
    *sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (*sock < 0) {
        logError("Falha ao criar o socket do cliente");
        return ERROR;
    }
    logInfo("Socket para cliente criado com sucesso");

    // Configura o endereço do server
    memset(serverAdress, 0, sizeof(*serverAdress));
    (*serverAdress).sin_family = AF_INET;
    (*serverAdress).sin_addr.s_addr = inet_addr(SERVER_IP);
    (*serverAdress).sin_port=htons(SERVER_PORT);
    logInfo("Endereço do servidor configurado com sucesso");

    logInfo("Cliente iniciado.");
    return SUCESS;
}

int main() {

    // Socket do cliente
    int clientSocket;
    // Endereço do servido
    struct sockaddr_in serverAdress;
    
    // Inicia cliente, associando a socket e configurando endereço do servidor
    int result = initClient(&clientSocket, &serverAdress);
    if(result == ERROR) {
        logError("Infelizmente não foi possível iniciar o cliente");
        exit(1);
    }

    // Inicia canhão
    // Envia sequencia de mensagens para servidor de 1 ao SEQUENCE_LIMIT
    int i = 1;
    while(i <= SEQUENCE_LIMIT) {
        char msg[MAX_MSG_SIZE];
        sprintf(msg, "%d", i);
        if (sendto(clientSocket, msg, strlen(msg), 0, (struct sockaddr*)&serverAdress, sizeof(serverAdress)) < 0) {
            logError("Falha ao enviar a mensagem");
            exit(1);
        }

        logInfo("Enviado: %s", msg);
        i++;
    }

    // Fecha o socket
    logInfo("Processo finalizado, fechando socket");
    close(clientSocket);

    return 0;
}
