#include <arpa/inet.h>
#include <asm-generic/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8080

int main(void) {
	int server_fd, client_fd;

	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("Socket creation failed");
		return EXIT_FAILURE;
	}

	struct sockaddr_in server_addr;

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);
	server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	int opt = 1;

	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt , sizeof(opt)) < 0) {
		perror("setsockopt failed");
		close(server_fd);
		return EXIT_FAILURE;
	}

	if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
		perror("Bind failed");
		close(server_fd);
		return EXIT_FAILURE;
	}

	if (listen(server_fd, 10) < 0) {
		perror("Listen failed");
		close(server_fd);
		return EXIT_FAILURE;
	}

	printf("Listening...\n");

	while (1) {
		struct sockaddr_in client_addr;
		socklen_t client_len = sizeof(client_addr);
		
		client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);

		if (client_fd < 0) {
			perror("Accept failed");
			continue;
		}

		printf("Connected: %s\n", inet_ntoa(client_addr.sin_addr));

		close(client_fd);
	}

	close(server_fd); // TODO: signal

	return 0;
}
