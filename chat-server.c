#include "utils.h"

int go = 1;

void Server(void);
void* Dispatcher(void*);
void* Worker(void*);

pthread_mutex_t logfileMutex;

void Server(){
	printf("server pid: %d\n", getpid());

	//Inizializzazione tabella utenti
	hash_t H = CREAHASH();
	loadUserfileInHash(H);

	//Inizializzazione logFile
	initLog();

	//Settando l'ambiente
	pthread_mutex_init(&logfileMutex, NULL);

	//Organizzazione socket server-side
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
		//Gestione lancio thread Worker
		newSocket = accept(sock, NULL, 0);
		pthread_t t;
		pthread_create(&t, 0, Worker, (void*)&newSocket);
		pthread_detach(t);
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
	char *message = calloc(SL, sizeof(char));
	read(*(int*)data, message, 256);
	fprintf(stderr, "Server received:%s\n", message);

	pthread_mutex_lock(&logfileMutex);
	writeMessageToLog("tiulalan", "ventupath", message);
	pthread_mutex_unlock(&logfileMutex);
	//while (go)
	//registra nel log-file in mutex
	//exec reg o ls | prod-cons con dispatcher (buffer circolare)
}

int main(int argc, char** argv) {
	pid_t p;
	p = fork();
	if(p == 0)
		Server();
	else if(p < 0)
		printf("fork fallita");
	exit(0);

}
