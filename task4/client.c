#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <signal.h>
#include <time.h>

#define MAX_SOCK_NAME 100
#define MAX_READ 10

char* conf = "config";

char* get_sock_name() {
    FILE* conf_f = fopen(conf, "r");
    char* sock_name = malloc(MAX_SOCK_NAME);
    fgets(sock_name, MAX_SOCK_NAME, conf_f);
    int path_len = snprintf(NULL, 0, "/tmp/%s", sock_name);
    char* sock_path = malloc(path_len+1);
    snprintf(sock_path, path_len+1, "/tmp/%s", sock_name);
    free(sock_name);
    return sock_path;
}

int get_rand(int lower, int upper) {
    return (rand() % (upper - lower + 1)) + lower;
}

int main(int argc, char** argv) {
    if (argc < 3) {
        printf("./client client_id nums_to_send");
        return 1;
    }

    int client_id = atoi(argv[1]);
    int nums_count = atoi(argv[2]);
    char* sock_name = get_sock_name();
    srand(time(0));
    float delay = 0;
    float sleep_time = get_rand(1, 255) * 1000;
    if (argc == 4) {
        delay = atof(argv[3]);
        sleep_time = delay * 1000000;
        printf("delay: %f\n", sleep_time);
    }
    printf("client: %d with delay: %f\n", client_id, sleep_time/1000000);
    struct sockaddr_un sockaddr;
    memset(&sockaddr, 0, sizeof(struct sockaddr_un));
    int sock_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sock_fd == -1) {
        perror("can't create socket");
        exit(1);
    }

    sockaddr.sun_family = AF_UNIX;
    strcpy(sockaddr.sun_path, sock_name);
    if (connect(sock_fd, (struct sockaddr*) &sockaddr, sizeof(sockaddr)) == -1){
        perror("can't connect to server");
        exit(1);
    }

    time_t t0 = time(0);

    for (int i = 0; i < nums_count; i++) {
        char send_buf[MAX_READ] = {0};
        char recv_buf[MAX_READ] = {0};
        scanf("%s", send_buf);
        usleep(sleep_time);
        write(sock_fd, send_buf, strlen(send_buf));
        read(sock_fd, recv_buf, MAX_READ);
    }

    time_t t1 = time(0);

    double time_in_seconds = difftime(t1, t0);
    printf("client time: %f\n", time_in_seconds);
    close(sock_fd);

    return 0;
}
