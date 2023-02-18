#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SERVER_IP "127.0.0.1"  // Endereço IP do servidor
#define SERVER_PORT 8888       // Porta do servidor
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
    if (inet_aton(SERVER_IP, &server_addr.sin_addr) == 0) {
        perror("Endereço IP inválido");
        exit(1);
    }

    // Envia uma mensagem para o servidor
    char msg[MAX_MSG_SIZE] = "Olá, servidor!";
    if (sendto(sock, msg, strlen(msg), 0, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Falha ao enviar a mensagem");
        exit(1);
    }

    printf("Mensagem enviada: %s\n", msg);

    // Fecha o socket
    close(sock);

    return 0;
}
