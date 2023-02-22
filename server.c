/**
 * @file server.c
 * @author Gustavo Henrique da Silva Barbosa (ghsb19) e Calebe Pompeo Helpa (cph19)
 * @brief Servidor do canhão UDP. Recebe diversas mensagens em sequência do cliente para calcular
 * a taxa de erro
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/types.h>

#include "logUtil.h"

#define MAX_MSG_SIZE 1024      // Tamanho máximo da mensagem
#define MAXHOSTNAME 30

#define ERROR -1
#define SUCCESS 1

#define SEQUENCE_LIMIT 100
#define BATCH_SIZE 100


/*!
  \brief Estrutura de dados que armazena informacoes sobre as mensagens recebidas
  \param totalMsgReceived Armazena o numero total de mensagens recebidas 
  \param lastMsg Armazena o numero de sequencia da ultima mensagem recebida
  \param lost Armazena o numero total de mensagens perdidas
  \param OutOfOrder Armazena o numero total de mensagens que chegaram fora de ordem
*/
typedef struct {
    int totalMsgReceived;   // Total de mensagens recebida
    int lastMsg;            // Valor da ultima mensagem
    //int totalMsgSent;       // Avaliar se faz sentido um campo que calcula o numero de msgs que deveria receber
                            // (totalMsgSent += (new - netInfo->lastMsg) para o caso de perda de msgs)
                            // Avaliar se ao final tem o mesmo valor que o client

    int lost; // Total de mesagens perdidas
    int outOfOrder; // Total de mensagens que chegaram fora de ordem
} net_info_t;


/*!
  \brief Configura o endereco do servidor, cria o socket e realiza o bind do socket com o endereco
  
  \param *sock 
  \param *serverAdress 
  \param serverPort 
  
  \return SUCCESS em caso de sucesso ou ERROR em caso de erro e mostra o motivo na saida padrao de erro
*/
int initServer (int *sock, struct sockaddr_in *serverAdress, int serverPort);


/*!
  \brief Recebe pacote do cliente
  
  \param socketServer
  \param *clientAdress
  \param *clientAdressLen
  
  \return Numero de sequencia do pacote recebido
*/
int newSequence (int socketServer, struct sockaddr_in *clientAdress, socklen_t *clientAdressLen);

/*!
  \brief Aloca memoria para net_info_t e inicializa seus campos com 0

  \return Ponteiro para a estrutura alocada em memoria
*/
net_info_t *initNetInfo();

/*!
  \brief Processa o numero de sequencia do pacote recebido do cliente. Define se houve perda de 
         pacotes, se o pacote recebido esta fora de ordem ou se nao houve erro.
  
  \param new Numero de sequencia do pacote recebido
  \param *netInfo Ponteiro para estrutura de dados que ira armazenar a informacao processada
  
  \return SUCCESS em caso de sucesso ou ERROR em caso de erro e mostra o motivo na saida padrao de erro
*/
int processMsg(int new, net_info_t *netInfo);

/*!
  \brief Determina a taxa de perda de pacotes, a taxa de pacotes que sao recebidos fora de ordem
         e imprime na tela juntamente com as infomacoes da estrutura de dados net_info
  
  \param *netInfo Ponteiro para estrutura de dados que armazena informacoes sobre os pacotes recebidos
*/
void printNetworkInfo(net_info_t *netInfo);

int main(int argc, char *argv[]) {

    fprintf(stdout, "==================================================================================");
    fprintf(stdout, "Inicio da execucao: programa que inicializa o servidor para execucao do canhao UDP");
    fprintf(stdout, "Gustavo Henrique da Silva Barbosa (ghsb19) e Calebe Pompeo Helpa (cph19)- Redes II");
    fprintf(stdout, "==================================================================================");

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

    // Informações sobre o server
    if(argc != 2) {
      logError("Uso correto: <ip-servidor> <porta>");
      exit(1);
    }
    int serverPort = atoi(argv[1]);
    
    // Inicia server
    int result = initServer(&socketServer, &serverAdress, serverPort);
    if(result == ERROR){
        logError("Não foi possível iniciar o servidor");
        exit(1);
    }

    // Recebe e processa as mensagens dos clientes
    int bytes_received;
    while (1) {
        int new = newSequence(socketServer, &clientAdress, &clientAdressLen);
        
        processMsg(new, netInfo);

        if(netInfo->totalMsgReceived % BATCH_SIZE == 0){
            printNetworkInfo(netInfo);
        }
    }

    // TODO: Fecha o socket
    // close(socketServer);

    return 0;
}

int processMsg(int new, net_info_t *netInfo) {
    netInfo->totalMsgReceived++;
    //netInfo->totalMsgSent++;

    if(new > (netInfo->lastMsg + 1)){

        // Log modificado - Testar para definir se esta melhor ou nao
        logWarning("Aparente perda de %d mensagens: última menagem recebida %d - mensagem atual %d", (new - netInfo->lastMsg), netInfo->lastMsg, new);

        // Se a mensagem não for a próxima, houve perda
        // É realizado o cálculo pois pode ser que seja um intevalo de mensagens
        netInfo->lost += (new - netInfo->lastMsg);
        //netInfo->totalMsgSent += (new - netInfo->lastMsg) - 1; //-1 por conta do netInfo->totalMsgSent++ acima
        netInfo->lastMsg = new;
        return ERROR;
    } else if (new < netInfo->lastMsg) {
        logWarning("Mensagen recebida fora de ordem: última menagem recebida %d - mensagem atual %d", netInfo->lastMsg, new);

        // Se a mensagem for anterior a última siginifica que chegou atrasada
        // Portanto uma das que estava perdida não está mais
        netInfo->lost--;
        netInfo->outOfOrder++;
        //netInfo->totalMsgSent--; //-1 porque ja tinha sido contada na aparente perda de mensagens
        return ERROR;
    } 
    
    netInfo->lastMsg = new;
    return SUCCESS;
}

void printNetworkInfo(net_info_t *netInfo) {
    double loss_rate, outOfOrder_rate;
    // loss_rate = 100.0*(double)netInfo->lost/(double)netInfo->totalMsgReceived; // Ou netInfo->totalMsgSent?
    // outOfOrder_rate = 100.0*(double)netInfo->outOfOrder/(double)netInfo->totalMsgReceived; // Ou netInfo->totalMsgSent?
    
    logInfo("Status:\n\tTotal de pacotes recebidos: %d [última = %d]\n\tTotal de perda de pacotes: %d []\n\tTotal de pacotes fora de ordem: %d []", netInfo->totalMsgReceived, netInfo->lastMsg, netInfo->lost, netInfo->outOfOrder);
    logInfo("\tTaxa de perda de pacotes: %lf%%\n\tTaxa de pacotes recebidos fora de ordem: %lf%%", loss_rate, outOfOrder_rate);
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

int initServer(int *sock, struct sockaddr_in *serverAdress, int serverPort) {
    logInfo("Iniciando o server...");
    struct hostent *hp;
    char localhost [MAXHOSTNAME];
    
    gethostname(localhost, MAXHOSTNAME);
    if ((hp = gethostbyname( localhost)) == NULL){
		logError("Nao consegui meu proprio IP");
		return ERROR;
	}

    // Configura o endereço do server
    (*serverAdress).sin_port = htons(serverPort);
	bcopy ((char *) hp->h_addr, (char *) &((*serverAdress).sin_addr), hp->h_length);
    (*serverAdress).sin_family = hp->h_addrtype;

    //TODO: Talvez remover
    // memset(serverAdress, 0, sizeof(*serverAdress));
    // (*serverAdress).sin_family = AF_INET;
    // (*serverAdress).sin_addr.s_addr = htonl(INADDR_ANY);
    logInfo("Endereço do server configurado com sucesso");

    // Cria um socket UDP para o server
    *sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (*sock < 0) {
        logError("Falha ao criar o socket do server");
        return ERROR;
    }
    logInfo("Socket para server criado com sucesso");

    // Associa o socket ao endereço do servidor
    if (bind(*sock, (struct sockaddr*)serverAdress, sizeof(*serverAdress)) < 0) {
        logError("Falha ao fazer o bind");
        return ERROR;
    }
    logInfo("Socket associado ao endereço do servidor com sucesso");


    logInfo("Server iniciado na porta %d.", serverPort);
    return SUCCESS;
}

int newSequence (int socketServer, struct sockaddr_in *clientAdress, socklen_t *clientAdressLen) {
    char msg[BUFSIZ];
    int bytes_received;

    // Recebe pacote e testa se recebeu corretamente
    bytes_received = recvfrom(socketServer, msg, BUFSIZ, 0, (struct sockaddr*)clientAdress, clientAdressLen);
    if (bytes_received < 0) {
        logError("Falha ao receber a mensagem");
        exit(1);
    }
    int sequence;
    sscanf(msg,"%d",&sequence);
    logInfo("Recebido pacote # %d.\n", sequence);

    //int sequence = atoi(msg);
    //printf("Mensagem recebida de %s:%d: %d\n", inet_ntoa(clientAdress->sin_addr), ntohs(clientAdress->sin_port), sequence);
    
    return sequence;
}
