#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#define OVER (-1)
#define max 2000
#define BUFFER_SIZE 100

typedef struct {
	char buffer[BUFFER_SIZE];
	pthread_mutex_t M;
	int readpos, writepos;
	int cont;
	pthread_cond_t PIENO;
	pthread_cond_t VUOTO;
} RingBuffer;


void init (RingBuffer * b) {
	pthread_mutex_init(&b->M,    NULL);
	pthread_cond_init(&b->PIENO, NULL);
	pthread_cond_init(&b->VUOTO, NULL);
	b->cont     = 0;
	b->readpos  = 0;
	b->writepos = 0;
}

//Le stringhe nel buffer circolare sono separate da un carattere nullo
//Riusciamo in questo modo a leggere solo i caratteri necessari alla comunicazione

void writeBuffer (char* content, RingBuffer *b) {
  pthread_mutex_lock(&b->M);
  
	while ( BUFFER_SIZE - b->cont <= strlen(content) + 1 )       // buffer pieno
		pthread_cond_wait(&b->PIENO, &b->M);

	strcpy(b->buffer+b->writepos, content);
	b->buffer[b->writepos + strlen(content) + 1 % BUFFER_SIZE] = '\0';

	b->cont += strlen(content) + 1;
	b->writepos = (b->writepos + strlen(content) + 1) % BUFFER_SIZE;
  
	pthread_cond_signal(&b->VUOTO);
	pthread_mutex_unlock(&b->M);
}


char* readBuffer (RingBuffer *b) {
	pthread_mutex_lock(&b->M);
	while (b->cont == 0) // buffer vuoto
		pthread_cond_wait(&b->VUOTO, &b->M);

	char* ret = b->buffer + b->readpos;
	b->cont -= strlen(ret) + 1;
	b->readpos = (b->readpos + strlen(ret) + 1) % BUFFER_SIZE;

	pthread_cond_signal(&b->PIENO);
	pthread_mutex_unlock(&b->M);
  
	return ret;
}
