#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SERVER_PORT 8888       // Porta do servidor
#define SERVER_IP "8888"       // IP do servidor
#define MAX_MSG_SIZE 1024      // Tamanho máximo da mensagem

int main() {
    // Cria um socket UDP
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("Falha ao criar o socket");
        exit(1);
    }

    // Configura o endereço do servidor
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

    // Associa o socket ao endereço do servidor
    if (bind(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Falha ao associar o socket ao endereço do servidor");
        exit(1);
    }

    printf("Servidor UDP inicializado na porta %d\n", SERVER_PORT);

    // Recebe e processa as mensagens dos clientes
    char msg[MAX_MSG_SIZE];
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    int bytes_received;
    while (1) {
        bytes_received = recvfrom(sock, msg, MAX_MSG_SIZE, 0, (struct sockaddr*)&client_addr, &client_addr_len);
        if (bytes_received < 0) {
            perror("Falha ao receber a mensagem");
            exit(1);
        }

        printf("Mensagem recebida de %s:%d: %s\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), msg);

        // Processa a mensagem recebida
        // ...

        // Envia uma resposta para o cliente
        char reply[MAX_MSG_SIZE] = "Mensagem recebida com sucesso!";
        if (sendto(sock, reply, strlen(reply), 0, (struct sockaddr*)&client_addr, client_addr_len) < 0) {
            perror("Falha ao enviar a resposta");
            exit(1);
        }

        printf("Resposta enviada para %s:%d: %s\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), reply);
    }

    // Fecha o socket
    close(sock);

    return 0;
}
