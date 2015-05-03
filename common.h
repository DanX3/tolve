#ifndef COMMON_H
#define COMMON_H

#define SL 256 // string length
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MSG_LOGIN	'L' //login
#define MSG_RELOG 	'R' //register and login
#define MSG_OK		'O' //OK
#define MSG_ERROR 	'E' //error
#define MSG_SINGLE 	'S' //message to single user
#define MSG_BRDCAST	'B' //message broadcast
#define MSG_LIST	'I' //list 
#define MSG_LOGOUT 	'X' //logout

typedef struct struct_hdata_t {
char * uname; // username
char * fullname; // full name
char * email; // email address
int sockid; // socket identifier; -1 = not connected
} hdata_t;

#endif

