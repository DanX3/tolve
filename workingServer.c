#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>


int main(int argc, char** argv) {
	int pid = fork();
	if (pid > 0) { //Father process
		char gomando[20];
		int sock, newSocket, figlio, status, i;
		struct sockaddr_in mio_ind;
		sock = socket(PF_INET, SOCK_STREAM, 0);

		mio_ind.sin_family = PF_INET;
		mio_ind.sin_addr.s_addr = INADDR_ANY;
		mio_ind.sin_port = 22375;

		if ( bind(sock, (struct sockaddr*)&mio_ind, sizeof(struct sockaddr_in)) < 0) {
			perror("bind");
			exit(1);
		}

		listen(sock, 5);

		newSocket = accept(sock, 0, 0);
		read(newSocket, gomando, sizeof(gomando));
		printf("%s\n", gomando);
		close (newSocket);
		close(sock);
		exit(0);
	} else { //Child process
		int sock, i;
		char mess[10], ris[1000];
		struct sockaddr_in rem_ind;

		rem_ind.sin_family = PF_INET;
		rem_ind.sin_addr.s_addr = inet_addr("127.0.0.1");
		rem_ind.sin_port = 22375;

		strcpy(mess, argv[1]);

		sock = socket(PF_INET, SOCK_STREAM, 0);
		connect(sock, (struct sockaddr*)&rem_ind, sizeof(struct sockaddr_in));
		if (write(sock, mess, sizeof(mess)) < 0) {
			perror("write");
			exit(1);
		}
		shutdown(sock, 2);
		exit(0);
	}
}
