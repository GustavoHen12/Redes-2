/**
 * @file client.c
 * @author Gustavo Henrique da Silva Barbosa (ghsb19) e Calebe Pompeo Helpa (cph19)
 * @brief Cliente do canhão UDP. Envia diversas mensagens em sequência para o servidor que irá calcular
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
#include <netdb.h>

#include "logUtil.h"

/*================================================================================*/
/*                                   CONSTANTES                                   */
/*================================================================================*/

#define ERROR -1
#define SUCESS 1

// #define SEQUENCE_LIMIT 2147483647
#define SEQUENCE_LIMIT 3000
#define BATCH_SIZE 100

/*================================================================================*/
/*                                HEADER DA FUNCAO                                */
/*================================================================================*/

/*!
  \brief Cria o socket para o cliente e configura endereco do server para enviar pacotes
  
  \param *sock Ponteiro que ira receber o descritor de arquivo do socket que sera aberto
  \param *serverAdress 
  \param *serverIp
  \param serverPort 
  
  \return SUCCESS em caso de sucesso ou ERROR em caso de erro e mostra o motivo na saida padrao de erro
*/
int initClient (int *sock, struct sockaddr_in *serverAdress, char *serverIp, int serverPort);

/*================================================================================*/
/*                                      MAIN                                      */
/*================================================================================*/

int main(int argc, char *argv[]) {

    fprintf(stdout, "==================================================================================");
    fprintf(stdout, "Inicio da execucao: programa que inicializa o cliente para execucao do canhao UDP.");
    fprintf(stdout, "Gustavo Henrique da Silva Barbosa (ghsb19) e Calebe Pompeo Helpa (cph19)- Redes II");
    fprintf(stdout, "==================================================================================");

    // Socket do cliente
    int clientSocket;
    // Endereço do servidor
    struct sockaddr_in serverAdress;
    
    // Informações sobre o server
    int serverPort;
    char *serverIp;

    if(argc != 4) {
      logError("Uso correto: <ip-servidor> <porta> <quantidade-mensagens>");
      exit(1);
    }

    serverPort = atoi(argv[2]);
    serverIp = argv[1];
    int msgLimit = atoi(argv[3]);

    // Inicia cliente, associando a socket e configurando endereço do servidor
    int result = initClient(&clientSocket, &serverAdress, serverIp, serverPort);
    if(result == ERROR) {
        logError("Infelizmente não foi possível iniciar o cliente");
        exit(1);
    }

    // Inicia canhão
    // Envia sequencia de mensagens para servidor de 1 ao SEQUENCE_LIMIT
    logInfo("Iniciando envio de pacotes...");
    int i = 1;
    while(i < msgLimit) {
        char msg[BUFSIZ+1];
        // Estamos enviado um número, que será recebido e convertido no servidor
        // entretanto, depois de alguns usos, considerando apenas números, ele pode pegar algum                 ->NAO ENTENDI MMUITO BEM
        // número que não deveria, portanto adicionamos um caracter qualquer para que ele pegue apenas o número
        sprintf(msg, "%da", i);

        // Envia mensagem criada
        if (sendto(clientSocket, msg, strlen(msg)+1, 0, (struct sockaddr*)&serverAdress, sizeof(serverAdress)) != strlen(msg)+1) {
            logError("Falha ao enviar a mensagem");
            exit(1);
        }

        // A cada BATCH_SIZE mensagens printa mensagem
        if(i % BATCH_SIZE == 0){
            logInfo("Enviado: %d mensagens", i);
        }
        i++;
    }
    logInfo("Pacotes enviados: %d", i);

    // Fecha o socket
    logInfo("Processo finalizado, fechando socket");
    // close(clientSocket);

    return 0;
}

/*================================================================================*/
/*                             IMPLEMENTACAO DA FUNCAO                            */
/*================================================================================*/

int initClient (int *sock, struct sockaddr_in *serverAdress, char *serverIp, int serverPort) {
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
    (*serverAdress).sin_addr.s_addr = inet_addr(serverIp);
    (*serverAdress).sin_port=htons(serverPort);

    logInfo("Endereço do servidor configurado com sucesso");

    logInfo("Cliente iniciado.");
    return SUCESS;
}