#include <arpa/inet.h>
#include <asm-generic/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define SERVER_PORT 8080
#define BACKLOG 10
#define REQUEST_BUFFER 4096

static int create_listening_socket(void);
static struct sockaddr_in create_server_addr(void);
static void server_loop(int server_fd);
static void handle_client(int client_fd, const struct sockaddr_in *client_addr);

int main(void) {
	int server_fd = create_listening_socket();

	if (server_fd < 0) {
		return EXIT_FAILURE;
	}

	server_loop(server_fd);

	close(server_fd); // TODO: signal

	return 0;
}

static struct sockaddr_in create_server_addr(void) {
	struct sockaddr_in addr = {0};

	addr.sin_family = AF_INET;
	addr.sin_port = htons(SERVER_PORT);
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	return addr;
}

static int create_listening_socket(void) {
	int server_fd = socket(AF_INET, SOCK_STREAM, 0);

	if (server_fd < 0) {
		perror("Socket creation failed");
		return -1;
	}

	struct sockaddr_in server_addr = create_server_addr();

	int opt = 1;
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt , sizeof opt) < 0) {
		perror("setsockopt failed");
		close(server_fd);
		return -1;
	}

	if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof server_addr) < 0) {
		perror("Bind failed");
		close(server_fd);
		return -1;
	}

	if (listen(server_fd, BACKLOG) < 0) {
		perror("Listen failed");
		close(server_fd);
		return -1;
	}

	printf("Listening on port %d\n", ntohs(server_addr.sin_port));

	return server_fd;
}

static void server_loop(int server_fd) {
	while (1) {
		struct sockaddr_in client_addr = {0};
		socklen_t client_len = sizeof client_addr;
		
		int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);

		if (client_fd < 0) {
			perror("Accept failed");
			continue;
		}

		handle_client(client_fd, &client_addr);
	}
}

static void handle_client(int client_fd, const struct sockaddr_in *client_addr) {
	printf("Client connected: %s:%d\n", inet_ntoa(client_addr->sin_addr), ntohs(client_addr->sin_port));

	char buffer[REQUEST_BUFFER] = {0};
	ssize_t received_bytes = recv(client_fd, buffer, sizeof buffer,0);
	
	if (received_bytes < 0) {
		perror("Recv failed");
	}
	else if (received_bytes == 0) {
		printf("Client disconnected.\n");
	}
	else {
		printf("%.*s\n", (int)received_bytes, buffer);

		const char *response =
			"HTTP/1.1 200 OK\r\n"
			"Content-Length: 9\r\n"
			"Content-Type: text/plain\r\n"
			"Connection: close\r\n"
			"\r\n"
			"greetings";

		send(client_fd, response, strlen(response), 0);
	}

	close(client_fd);
}
