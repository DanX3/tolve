#include "utils.h"

int main(int argc, char** argv) {
	printf("client pid: %d\n", getpid());
	struct sockaddr_in server;
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	
	bzero(&server, sizeof(struct sockaddr_in));
	server.sin_family = AF_INET;
	server.sin_port = htons(5001);
	server.sin_addr.s_addr = inet_addr("127.0.0.1");
	
	connect(sock, (struct sockaddr*) &server, sizeof(struct sockaddr_in));
	fprintf(stderr, "Client says:%s\n", argv[1]);
	write(sock, argv[1], 256);
	exit(0);
}

