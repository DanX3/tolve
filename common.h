#ifndef COMMON_H
#define COMMON_H

#define SL 256 // string length
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MSG_LOGIN	'L' //login
#define MSG_REGLOG 	'R' //register and login
#define MSG_OK		'O' //OK
#define MSG_ERROR 	'E' //error
#define MSG_SINGLE 	'S' //message to single user
#define MSG_BRDCAST	'B' //message broadcast
#define MSG_LIST	'I' //list 
#define MSG_LOGOUT 	'X' //logout
#define CMD_LOGOUT 	"#x"
#define CMD_LIST 	"#ls"
#define CMD_SEND 	"#dest"
#define HELP_PATH 	"help.txt"

#define max 2000
#define BUFFER_SIZE 1000

typedef struct struct_hdata_t {
	char * uname; // username
	char * fullname; // full name
	char * email; // email address
	int sockid; // socket identifier; -1 = not connected
} hdata_t;

typedef struct __msg_t__ {
	char type;
	char *sender;
	char *receiver;
	unsigned int msglen;
	char *content;
} msg_t;

typedef struct {
	char buffer[BUFFER_SIZE];
	pthread_mutex_t M;
	int readpos, writepos;
	int cont;
	pthread_cond_t PIENO;
	pthread_cond_t VUOTO;
} RingBuffer;

#endif

