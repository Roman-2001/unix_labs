#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/time.h>
#include <stdarg.h>

#define MAX_CLIENTS 150
#define MAX_SOCK_NAME 100
#define MAX_READ 10

char *conf = "config";
int state = 0;
FILE* log_file;

void write_log(char* format, ...) {
    va_list va;
    va_start(va, format);
    int log_len = vsnprintf(NULL, 0, format, va);
    va_end(va);
    char* log_line = malloc(log_len + 1);
    va_start(va, format);
    vsnprintf(log_line, log_len + 1, format, va);
    va_end(va);
    fwrite(log_line, 1, log_len, log_file);
    fflush(log_file);
    free(log_line);
}

void open_log() {
    	log_file = fopen("/tmp/server.log", "w");
}

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

int make_socket() {
    	int server_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    	struct sockaddr_un addr;
    	memset(&addr, 0, sizeof(struct sockaddr_un));

    	addr.sun_family = AF_UNIX;
    	char* sock_name = get_sock_name();
    	strcpy(addr.sun_path, sock_name);

    	int option = 0;
    	setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (char*) &option, sizeof(option));

    	int bind_res = bind(server_socket, (struct sockaddr*) &addr, sizeof(addr));
    	if (bind_res == -1) {
        	perror("can't bind socket");
        	exit(1);
    	}

    	int listen_res = listen(server_socket, MAX_CLIENTS);
    	if (listen_res == -1) {
        	perror("can't listen socket");
        	exit(1);
    	}
    	return server_socket;
}


int main() {

    	open_log();
    	int server_socket = make_socket();
    	write_log("server started\n");
    	fd_set readfds;
    	int client_sockets[MAX_CLIENTS];
    	for (int i = 0; i < MAX_CLIENTS; i++) {
        	client_sockets[i] = 0;
    	}
    
    	while (1) {
		
        	FD_ZERO(&readfds);
        	FD_SET(server_socket, &readfds);
        	int max_fd = server_socket;
        	for (int i = 0; i < MAX_CLIENTS; i++) {
        		int sd = client_sockets[i];
            		if (client_sockets[i] > 0) {
               	 	FD_SET(sd, &readfds);
            		}
            		if (sd > max_fd) {
               		max_fd = sd;
            		}
        	}
        	int select_res = select(max_fd + 1, &readfds, NULL, NULL, NULL);
        	if (select_res < 0) {
            		printf("error during select\n");
        	}
        	if (FD_ISSET(server_socket, &readfds)) {
            		int client_sock = accept(server_socket, NULL, NULL);
            		for (int i = 0; i < MAX_CLIENTS; i++) {
                		if (client_sockets[i] == 0) {
                    			client_sockets[i] = client_sock;
                    			long sbrk_val = (long) sbrk(0);
                    			write_log("Accept client socket %d. Current sbrk() = %d\n", client_sockets[i], sbrk_val);
                    			break;
                		}
            		}
        	}
        	for (int i = 0; i < MAX_CLIENTS; i++) {
            		if (FD_ISSET(client_sockets[i], &readfds)) {
            			
                		char read_buf[MAX_READ] = {0};
                		int read = recv(client_sockets[i], read_buf, MAX_READ, 0);
                		if (read <= 0) {
                    			close(client_sockets[i]);
                    			client_sockets[i] = 0;
                    			continue;
                		}

                		int recv_num = atoi(read_buf);
                		state += recv_num;
                		write_log("recv number: %d; send state: %d\n", recv_num, state);
                		char write_buf[MAX_READ] = {0};
                		sprintf(write_buf, "%d", state);
                		send(client_sockets[i], write_buf, strlen(write_buf), 0);
            		}
        	}
    	}
    	close(server_socket);
    	return 0;
}
