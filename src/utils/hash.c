/*************************************/
/* Library hash: schifano fabio 2013 */
/*************************************/
#include "lista.c"
/* numero primo */
#define HL 997


//////////////////////////////////////////////////////////////////////

int hashfunc(char * k) {
  int i = 0;
  int hashval = 0;
  for ( i=0; i < strlen(k); i++ ) {
    hashval = ((hashval*SL) + k[i]) % HL;
  }
  return hashval;
}

//////////////////////////////////////////////////////////////////////
// restituisce la posizione della cella che contiene la chiave key == uname

hdata_t * CERCALISTA ( char * key, lista L ) {
  int found;
  posizione p;
  hdata_t * r;
  char * k;
  r = NULL;
  p = PRIMOLISTA(L);
  found = 0;
  //fprintf(stderr, "looking for key: %s\n", key);
  while ( (!found) && (!FINELISTA(p,L)) ) {
    k = ((hdata_t *)(p->elemento))->uname;
    if ( strcmp (k, key ) == 0 ) {
      r = p->elemento;
    }
    p = SUCCLISTA(p);
  }
  return r;
}

//////////////////////////////////////////////////////////////////////

hash_t CREAHASH() {
  int i;
  hash_t hashTable = (hash_t) malloc(HL*sizeof(lista));
  for ( i=0; i < HL; i++ )
    hashTable[i] = CREALISTA();
  return hashTable;
}

//////////////////////////////////////////////////////////////////////
// cerca un elemento nella tabella Hash
// restituisce un "hdata_t *" oppure NULL se non esiste

void * CERCAHASH(char * key, hash_t hashTable) {
  int i;
  i = hashfunc(key);
  return CERCALISTA(key, hashTable[i]);
}

//////////////////////////////////////////////////////////////////////
// inserisce l'elemento hdata_t * nella tabella hash H

int INSERISCIHASH (char * key, hdata_t * elem, hash_t hashTable) {
  int i;
  posizione p;
  i = hashfunc(key);
  if ( CERCAHASH(key, hashTable) == NULL ) {
    p = hashTable[i];
    INSLISTA((void *)elem, &p);
    return 1;
  }
  return 0;
}

//////////////////////////////////////////////////////////////////////
void DISTRUGGIHASH(hash_t hashTable) {
	int i;
	for ( i=0; i < HL; i++ )
		free(hashTable[i]);
	free(hashTable);
}
