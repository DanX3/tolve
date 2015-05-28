#include "utils.h"

int go = 1;
int activeThreads = 0;
RingBuffer* ringBuffer;

void Server(char*, char*);
void* Dispatcher(void*);
void* Worker(void*);

pthread_mutex_t logfileMutex;
pthread_mutex_t activeThreadsMutex;
hash_t H;
StringList loggedList;
int sock, newSocket;

void signalHandler(int signum) {
	saveHashInUserfile(H);
	close(sock);
	printf("Server interrupted: signal (%d) received\n", signum);
	exit(0);
}

void Server(char* userfile, char* logfile){
	printf("server pid: %d\n", getpid());

	//Settando l'ambiente
	H = CREAHASH();
	printf("#\n");
	loadUserfileInHash(H, userfile);
	pthread_mutex_init(&logfileMutex, NULL);
	initLog(logfile);
	loggedList = initLoggedUser();
	ringBuffer = malloc(sizeof(RingBuffer));
	initRingBuffer(ringBuffer);
	signal(SIGTERM, signalHandler);
	signal(SIGINT, signalHandler);

	//Organizzazione socket server-side
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
#if 1
	pthread_t dispatcher;
	pthread_create(&dispatcher, 0, Dispatcher, 0);
#endif	
	while(go){
		newSocket = accept(sock, NULL, 0);
		pthread_t t;
		pthread_create(&t, 0, Worker, (void*)&newSocket);
		pthread_detach(t);
	}
	pthread_join(dispatcher, 0);
}

void*  Dispatcher(void* data) {
	char* stringMessage = calloc(SL, sizeof(char));
	msg_t* msg = calloc(1, sizeof(msg_t));
	while (go) {
		msg = unMarshal(readBuffer(ringBuffer));
		switch(msg->type) {
		case MSG_SINGLE:
			write(getDataFrom(msg->receiver, H)->sockid, marshal(msg), SL);
			break;
		case MSG_BRDCAST: {
			//SCBroadcast(msg->sender, msg->content, msg);
			int usersWritten = 0, i = -1;
			char *currentUser;
			while (usersWritten < activeThreads - 1) {
				i++;
				if (!existsUserAt(i, loggedList))		continue;
				currentUser = getUserAt(i, loggedList);
				if ( strcmp( currentUser, msg->sender) == 0 )	continue;

				//If the receiver is correct, send to him the message
				pthread_mutex_lock(&logfileMutex);
				writeMessageToLog(	msg->sender,
							getDataFrom(currentUser, H)->uname, 
							msg->content);
				pthread_mutex_unlock(&logfileMutex);

				write( getDataFrom(currentUser, H)->sockid, marshal(msg), SL);
				usersWritten++;
			}
			break;
		}
		}
	}
}

void*  Worker(void* data) {
	int socket = *(int*)data;
	char *input = calloc(SL, sizeof(char));
	msg_t* msg = calloc(1, sizeof(msg_t));
	char *username = calloc(SL, sizeof(char));


	while (go) {
		if ( read(socket, input, SL) == 0 )
			exit(1);
		msg = unMarshal(input);
		switch(msg->type) {
		case MSG_LOGIN:
			username = msg->content;
			strcpy(username, msg->content);
			bzero(msg, sizeof(msg_t));
			if ( checkLoggedUser(username, loggedList) ) {
				SCError(LOGIN_DONE_YET, msg);
				writeErrorToLog(LOGIN_DONE_YET, username);
				write(socket, marshal(msg), SL);
				pthread_exit(0);
			}

			if ( CERCAHASH(username, H) == 0 ) {
				SCError(USER_NOT_REGISTERED, msg);
				writeErrorToLog(USER_NOT_REGISTERED, username);
				write(socket, marshal(msg), SL);
				pthread_exit(0);
			} 

			SCOK(msg);
			write(socket, marshal(msg), SL);
			addLoggedUser(username, loggedList);
			getDataFrom(username, H)->sockid = socket;
			writeAccessToLog(1, username);

			pthread_mutex_lock(&activeThreadsMutex);
			activeThreads++;
			pthread_mutex_unlock(&activeThreadsMutex);
			break;
		case MSG_REGLOG: {
			hdata_t *userInfo = string2hdata(msg->content);
			if ( INSERISCIHASH(userInfo->uname, userInfo, H) == 0 ) {
				SCError(HASH_COLLISION, msg);
				writeErrorToLog(HASH_COLLISION, username);
			} else 
				SCOK(msg);

			write(socket, marshal(msg), SL);
			break;
		}
		case MSG_SINGLE:
			if ( CERCAHASH(msg->receiver, H) == 0 ) {
				SCError(RECV_NOT_REGISTERED, msg);
				pthread_mutex_lock(&activeThreadsMutex);
				writeErrorToLog(RECV_NOT_REGISTERED, username);
				pthread_mutex_unlock(&activeThreadsMutex);

				write(socket, marshal(msg), SL);
			} else
			if ( checkLoggedUser(msg->receiver, loggedList) == 0) {
				SCError(RECV_OFFLINE, msg);
				pthread_mutex_lock(&activeThreadsMutex);
				writeErrorToLog(RECV_OFFLINE, username);
				pthread_mutex_unlock(&activeThreadsMutex);
				write(socket, marshal(msg), SL);
			} else {
				/*
				int recvSock = getDataFrom(msg->receiver, H)->sockid;
				SCSingle(username, msg->content, msg);
				write(recvSock, marshal(msg), SL);
				*/

				msg->sender = username;
				writeBuffer(marshal(msg), ringBuffer);

				pthread_mutex_lock(&logfileMutex);
				writeMessageToLog(username,  msg->receiver, msg->content);
				pthread_mutex_unlock(&logfileMutex);

			}
			break;
		case MSG_BRDCAST:
			SCBroadcast(username, msg->content, msg);
			writeBuffer(marshal(msg), ringBuffer);
			break;
		case MSG_LIST:
			SCList(listLoggedUser(loggedList), msg);
			write(socket, marshal(msg), SL);

			break;
		case MSG_LOGOUT:
			removeLoggedUser(username, loggedList);
			getDataFrom(username, H)->sockid = -1;
			pthread_mutex_lock(&activeThreadsMutex);
			writeAccessToLog(0, username);
			pthread_mutex_unlock(&activeThreadsMutex);
			free(input);
			free(msg);

			pthread_mutex_lock(&activeThreadsMutex);
			activeThreads--;
			pthread_mutex_unlock(&activeThreadsMutex);

			pthread_exit(0);
		}
		bzero(input, sizeof(input));
	}
}



int main(int argc, char** argv) {
	pid_t p;
	p = fork();
	if(p == 0)
		Server(argv[1], argv[2]);
	else if(p < 0)
		printf("fork fallita");
	exit(0);

}
