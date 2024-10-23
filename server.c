#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/ip.h>

#define TAMANHO_BUFFER 4096
#define PORTA_SERVIDOR 8080

// Estrutura do nosso "UDP" simulado
struct udp_simulado {
    uint16_t porta_origem;
    uint16_t porta_destino;
    uint16_t comprimento;
    uint16_t checksum;
    char dados[TAMANHO_BUFFER];
};

// Função para calcular o checksum
unsigned short checksum(void *b, int len) {
    unsigned short *buf = b;
    unsigned int sum = 0;
    for (sum = 0; len > 1; len -= 2)
        sum += *buf++;
    if (len == 1)
        sum += *(unsigned char *)buf;
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    return (unsigned short)(~sum);
}

int main() {
    struct sockaddr_in cliente;
    int sock;
    char buffer[TAMANHO_BUFFER];
    struct iphdr *ip_header;
    struct udp_simulado *udp_payload;
    socklen_t tamanho_cliente;
    int bytes_recebidos;

    // Cria um socket RAW
    sock = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
    if (sock < 0) {
        perror("Erro ao criar socket");
        return 1;
    }

    printf("Servidor esperando pacotes...\n");

    // Loop de recebimento
    while (1) {
        memset(buffer, 0, TAMANHO_BUFFER);  // Limpa o buffer
        tamanho_cliente = sizeof(cliente);

        // Recebe o pacote RAW
        bytes_recebidos = recvfrom(sock, buffer, TAMANHO_BUFFER, 0, (struct sockaddr *)&cliente, &tamanho_cliente);
        if (bytes_recebidos < 0) {
            perror("Erro ao receber pacote");
            return 1;
        }

        // Analisa o cabeçalho IP
        ip_header = (struct iphdr *)buffer;
        udp_payload = (struct udp_simulado *)(buffer + ip_header->ihl * 4);  // Pula o cabeçalho IP

        // Verifica o tamanho do pacote
        int tamanho_total = ntohs(ip_header->tot_len);
        if (tamanho_total != (sizeof(struct iphdr) + sizeof(struct udp_simulado))) {
            printf("Erro: Tamanho do pacote incorreto!\n");
            continue;
        }

        // Verifica o checksum
        uint16_t checksum_recebido = udp_payload->checksum;
        udp_payload->checksum = 0;  // Zera o checksum antes de recalcular
        uint16_t checksum_calculado = checksum(udp_payload, sizeof(struct udp_simulado) - TAMANHO_BUFFER + strlen(udp_payload->dados));

        if (checksum_recebido != checksum_calculado) {
            printf("Erro: Checksum incorreto!\n");
            continue;
        }

        printf("Pacote recebido!\n");
        printf("Origem: %d\n", ntohs(udp_payload->porta_origem));
        printf("Destino: %d\n", ntohs(udp_payload->porta_destino));
        printf("Dados: %s\n", udp_payload->dados);
    }

    close(sock);
    return 0;
}
