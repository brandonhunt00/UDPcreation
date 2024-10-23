#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/ip.h>

#define TAMANHO_BUFFER 1024
#define PORTA_DESTINO 8080

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
    struct sockaddr_in destino;
    int sock;
    char pacote[4096];  // Buffer para o pacote completo (cabeçalho IP + dados)
    struct iphdr *ip_header = (struct iphdr *)pacote;  // Cabeçalho IP
    struct udp_simulado *udp_payload = (struct udp_simulado *)(pacote + sizeof(struct iphdr));  // Dados do UDP simulado
    socklen_t tamanho_destino;

    // Cria um socket RAW
    sock = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
    if (sock < 0) {
        perror("Erro ao criar socket");
        return 1;
    }

    // Preenche o cabeçalho do nosso "UDP" personalizado
    udp_payload->porta_origem = htons(12345);  // Porta de origem fictícia
    udp_payload->porta_destino = htons(PORTA_DESTINO);  // Porta de destino
    udp_payload->comprimento = htons(sizeof(struct udp_simulado));  // Comprimento do pacote
    udp_payload->checksum = 0;  // Inicialmente 0

    // Preenche os dados
    strcpy(udp_payload->dados, "Olá, este é um pacote UDP simulado!");

    // Calcula o checksum para o payload UDP simulado (sem o checksum incluído)
    udp_payload->checksum = checksum(udp_payload, sizeof(struct udp_simulado) - TAMANHO_BUFFER + strlen(udp_payload->dados));

    // Preenche o cabeçalho IP
    ip_header->ihl = 5;  // Tamanho do cabeçalho IP (5 palavras de 32 bits)
    ip_header->version = 4;  // IPv4
    ip_header->tos = 0;  // Tipo de serviço
    ip_header->tot_len = htons(sizeof(struct iphdr) + sizeof(struct udp_simulado));  // Comprimento total do pacote
    ip_header->id = htonl(54321);  // Identificação do pacote
    ip_header->frag_off = 0;  // Sem fragmentação
    ip_header->ttl = 64;  // Time to live
    ip_header->protocol = IPPROTO_RAW;  // Protocolo RAW
    ip_header->check = 0;  // Checksum (preenchido depois)
    ip_header->saddr = inet_addr("127.0.0.1");  // Endereço de origem
    ip_header->daddr = inet_addr("127.0.0.1");  // Endereço de destino

    // Calcula o checksum do cabeçalho IP
    ip_header->check = checksum((unsigned short *)pacote, sizeof(struct iphdr));

    // Preenche a estrutura de destino
    destino.sin_family = AF_INET;
    destino.sin_port = htons(PORTA_DESTINO);
    destino.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Envia o pacote via socket RAW
    if (sendto(sock, pacote, ntohs(ip_header->tot_len), 0, (struct sockaddr *)&destino, sizeof(destino)) < 0) {
        perror("Erro ao enviar pacote");
        return 1;
    }

    printf("Pacote enviado com sucesso!\n");

    close(sock);
    return 0;
}
