#include "utils.h"

int go = 1;

void Server(char* userFile, char* logFile){
	printf("server pid: %d\n", getpid());
	int sock, newSocket;
	struct sockaddr_in my_addr;
	bzero(&my_addr,sizeof(struct sockaddr_in));
	sock = socket(AF_INET, SOCK_STREAM, 0);
	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(5001);
	my_addr.sin_addr.s_addr = INADDR_ANY;
	if (bind(sock, (struct sockaddr *) &my_addr, sizeof(my_addr)) < 0) {
		perror("bind");
		exit(-1);	
	}	
	listen(sock, 10);
	while(go){
		newSocket = accept(sock, NULL, 0);
		char message[256];
		bzero(message, sizeof(message));
		read(newSocket, message, 256);
		fprintf(stderr, "Server received:%s\n", message);
		//lancia un worker detached
		//termina su SIGTERM o SIGINT (go = 0)
	}
	close(sock);
	exit(0);
}

void*  Dispatcher(void* data) {
	//while (go)
	//prod-cons buffer circolare
	//controlla se gli utenti sono online (hash table in mutex)
	//nel caso invia il messaggio tramite socket
}

void*  Worker(void* data) {
	//while (go)
	//registra nel log-file in mutex
	//exec reg o ls | prod-cons con dispatcher (buffer circolare)
}

int main(int argc, char** argv) {
	pid_t p;
	p = fork();
	if(p==0)
		Server(userFile, logFile);
	else if(p < 0)
		printf("fork fallita");
	exit(0);

}
