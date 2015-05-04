#include "utils.h"

int main(int argc, char** argv) {

	if( strcmp(argv[1], "-h") == 0){
		printf("Scriviamo l'aiuto\n");
		exit(0);
	}

	struct sockaddr_in server; 
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	
	bzero(&server, sizeof(struct sockaddr_in));
	server.sin_family = AF_INET;
	server.sin_port = htons(5001);
	server.sin_addr.s_addr = inet_addr("127.0.0.1");
	
	connect(sock, (struct sockaddr*) &server, sizeof(struct sockaddr_in));
	char* sender;
	do {
		sender = calloc(SL, sizeof(char));
		size_t len = SL;
		getline(&sender, &len, stdin);
		sender = strtok(sender, "\n");
		write(sock, sender, SL);
	} while(strcmp(sender, "exit") != 0);
	exit(0);
}

