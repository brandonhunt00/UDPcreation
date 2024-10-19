#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

// Definição de cabeçalho IP simplificado
struct ip_header {
    uint8_t  version_ihl;    // Versão e comprimento do cabeçalho IP
    uint8_t  tos;            // Tipo de serviço
    uint16_t total_length;    // Comprimento total (IP + UDP + Dados)
    uint16_t id;             // Identificação do pacote
    uint16_t flags_offset;   // Flags e fragmentação
    uint8_t  ttl;            // Time to Live (TTL)
    uint8_t  protocol;       // Protocolo (UDP = 17)
    uint16_t checksum;       // Checksum do cabeçalho IP
    uint32_t source_ip;      // Endereço IP de origem
    uint32_t dest_ip;        // Endereço IP de destino
};

// Definição de cabeçalho UDP simplificado
struct udp_header {
    uint16_t source_port;   // Porta de origem
    uint16_t dest_port;     // Porta de destino
    uint16_t length;        // Comprimento total do UDP (cabeçalho + dados)
    uint16_t checksum;      // Checksum do cabeçalho UDP e dados
};

// Estrutura de pacote completo (IP + UDP + Dados)
struct udp_packet {
    struct ip_header ip_hdr;    // Cabeçalho IP
    struct udp_header udp_hdr;  // Cabeçalho UDP
    char data[1024];            // Dados (payload)
};

// Função para calcular checksum simplificado
uint16_t calculate_checksum(uint16_t *buffer, int size) {
    uint32_t sum = 0;
    while (size > 1) {
        sum += *buffer++;
        size -= 2;
    }
    if (size == 1) {
        sum += *(uint8_t *)buffer;
    }
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    return (uint16_t)(~sum);
}

// Função para criar e enviar pacotes UDP (simulação)
void send_udp_packet(struct udp_packet *packet, const char *dest_ip, const char *data, size_t data_len) {
    // Configurar o cabeçalho IP
    packet->ip_hdr.version_ihl = (4 << 4) | (sizeof(struct ip_header) / 4);
    packet->ip_hdr.tos = 0;
    packet->ip_hdr.total_length = htons(sizeof(struct ip_header) + sizeof(struct udp_header) + data_len);
    packet->ip_hdr.id = htons(54321);
    packet->ip_hdr.flags_offset = 0;
    packet->ip_hdr.ttl = 64;
    packet->ip_hdr.protocol = 17;  // Protocolo UDP (17)
    packet->ip_hdr.source_ip = inet_addr("192.168.1.10");  // IP fictício de origem
    packet->ip_hdr.dest_ip = inet_addr(dest_ip);  // IP de destino fornecido
    packet->ip_hdr.checksum = 0;
    packet->ip_hdr.checksum = calculate_checksum((uint16_t *)&packet->ip_hdr, sizeof(struct ip_header));

    // Configurar o cabeçalho UDP
    packet->udp_hdr.source_port = htons(12345);   // Porta de origem fictícia
    packet->udp_hdr.dest_port = htons(80);        // Porta de destino fictícia
    packet->udp_hdr.length = htons(sizeof(struct udp_header) + data_len);
    packet->udp_hdr.checksum = 0;  // Checksum é opcional no UDP

    // Copiar os dados para o pacote
    memcpy(packet->data, data, data_len);

    // Simular o envio do pacote para a rede
    printf("Pacote UDP enviado para %s\n", dest_ip);
    printf("Dados enviados: %s\n", data);
}

// Função para simular recepção de pacotes UDP
void receive_udp_packet(struct udp_packet *packet) {
    // Simular recepção de um pacote
    printf("Pacote UDP recebido:\n");
    printf("De: %s\n", inet_ntoa(*(struct in_addr *)&packet->ip_hdr.source_ip));
    printf("Para: %s\n", inet_ntoa(*(struct in_addr *)&packet->ip_hdr.dest_ip));
    printf("Dados: %s\n", packet->data);
}

// Thread para simular recebimento de pacotes
void* receivePackets(void *vargp) {
    // Criar um pacote UDP fictício para simulação
    struct udp_packet received_packet;
    strcpy(received_packet.data, "Simulação de pacote recebido");
    
    // Receber pacotes (simulação)
    receive_udp_packet(&received_packet);
    pthread_exit(NULL);
}

// Função principal simulando o servidor UDP
int main(void) {
    pthread_t thread_id;

    // Enviar pacotes UDP (simulação)
    struct udp_packet packet;
    const char *data = "Teste de dados UDP manual";
    send_udp_packet(&packet, "192.168.1.20", data, strlen(data));

    // Criar uma thread para simular a recepção de pacotes
    pthread_create(&thread_id, NULL, receivePackets, NULL);

    // Esperar o término da thread
    pthread_join(thread_id, NULL);

    printf("Servidor UDP: Processamento de pacotes finalizado.\n");
    return 0;
}
