#ifndef Stemmer_H
#define Stemmer_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>  /* for malloc, free */
#include <string.h>  /* for memcmp, memmove */

struct stemmer;

extern struct stemmer * create_stemmer(void);
extern void free_stemmer(struct stemmer * z);
extern int stem(struct stemmer * z, char * b, int k);

#ifdef __cplusplus
}
#endif

#endif