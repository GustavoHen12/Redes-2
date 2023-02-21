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
#define BATCH_SIZE 100

typedef struct {
    int totalMsgReceived; // Total de mensagens recebida
    int lastMsg; // Valor da ultima mensagem

    int lost; // Total de mesagens perdidas
    int outOfOrder; // Total de mensagens que chegaram fora de ordem
} net_info_t;


int initServer (int *sock, struct sockaddr_in *serverAdress);

int newSequence (int socketServer, struct sockaddr_in *clientAdress, socklen_t *clientAdressLen);

net_info_t *initNetInfo();

void processMsg(int new, net_info_t *netInfo);

void printNetworkInfo(net_info_t *netInfo);

int main() {
    // Informações do cliente
    struct sockaddr_in clientAdress;
    socklen_t clientAdressLen = sizeof(clientAdress);

    // Informações do server
    int socketServer;
    struct sockaddr_in serverAdress;

    // Informações da rede
    net_info_t *netInfo = initNetInfo();
    if(!netInfo){
        logError("Não foi possível iniciar as informações da rede !");
        exit(1);
    }

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
        
        processMsg(new, netInfo);

        if(netInfo->totalMsgReceived % BATCH_SIZE == 0){
            printNetworkInfo(netInfo);
        }
    }

    // Fecha o socket
    close(socketServer);

    return 0;
}

void processMsg(int new, net_info_t *netInfo) {
    if(new > (netInfo->lastMsg + 1)){
        logWarning("Aparente perda de mensagens, chegou a mensagem %d quando a última foi %d", new, netInfo->lastMsg);

        // Se a mensagem não for a próxima, houve alguma perca
        // É realizado o cálculo pois pode ser que seja um intevalo de mensagens
        netInfo->lost += (new - netInfo->lastMsg);
        netInfo->lastMsg = new;
    } else if (new < netInfo->lastMsg) {
        logWarning("Recebido mensagens fora de ordem, chegou a mensagem %d quando a última foi %d", new, netInfo->lastMsg);

        // Se a mensagem for anterior a última siginifica que chegou atrasada
        // Portanto uma das que estava perdida não está mais
        netInfo->lost--;
        netInfo->outOfOrder++;
    } else {
        netInfo->lastMsg = new;
    }

    netInfo->totalMsgReceived++;
}

void printNetworkInfo(net_info_t *netInfo) {
    logInfo("Status:\n\tTotal de mensagens: %d [última = %d]\n\tTotal de perca: %d []\n\tTotal fora de ordem: %d []", netInfo->totalMsgReceived, netInfo->lastMsg, netInfo->lost, netInfo->outOfOrder);
}

net_info_t *initNetInfo() {
    net_info_t *netInfo = malloc(sizeof(net_info_t));
    if(!netInfo){
        return NULL;
    }

    netInfo->lastMsg = 0;
    netInfo->totalMsgReceived = 0;
    netInfo->lost = 0;
    netInfo->outOfOrder = 0;
    return netInfo;
}

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
    // printf("Mensagem recebida de %s:%d: %d\n", inet_ntoa(clientAdress->sin_addr), ntohs(clientAdress->sin_port), sequence);
    
    return sequence;
}
