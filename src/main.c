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

	struct sockaddr_in addr;

	addr.sin_family = AF_INET;
	addr.sin_port = htons(PORT);
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	int opt = 1;

	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt , sizeof(opt)) < 0) {
		perror("setsockopt failed");
		close(server_fd);
		return EXIT_FAILURE;
	}

	if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
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
	
	if ((client_fd = accept(server_fd, NULL, NULL)) < 0) {
		perror("Accept failed");
	} else {
		printf("Connection successful.\n");
	}

	close(client_fd);
	close(server_fd);

	return 0;
}
