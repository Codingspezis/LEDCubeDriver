#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include <sys/socket.h>
#include <netinet/in.h>

#include "illuminator.h"

int g_sock;

int initServer(int port) {
	struct sockaddr_in serverAddr;
	g_sock = socket(AF_INET, SOCK_STREAM, 0);
	if(g_sock < 0) return -1;
	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddr.sin_port=htons(port);
	return bind(g_sock, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
}

void handleClient(int fd) {
	printf("new client accepted\n");
	uint8_t buffer[16];
	while(read(fd, buffer, 16) == 16)
		memcpy(cubeState, buffer, 16);
	close(fd);
}

void listen4Clients() {
	int clientFD;
	if(!listen(g_sock, 10)) {
		for(;;) {
			clientFD = accept(g_sock, (struct sockaddr*)NULL, NULL);
			if(clientFD)
				handleClient(clientFD);
		}
	}
}
