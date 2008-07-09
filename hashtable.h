/* hashtable.h
**
** Per-Erik Martin (per-erik.martin@telia.com) 2001-05-13
**
*/

#ifndef _hashtable_h_
#define _hashtable_h_

#include <stddef.h>

typedef struct hashtable_s *hashtable_t;

/* The type for a string hash function */
typedef unsigned long
hashfunc_t(const char *s);

/* A type for a destructor function for the value data */
typedef void
hashdestfunc_t(void *);

/* This is a reasonably good, and fast string hash function */
unsigned long
hash_string_fast(const char *s);

/* This one is very good, esp. on difficults sets of keys, but
** about twice as slow as hash_string_fast()
*/
unsigned long
hash_string_good(const char *s);

/* Create a hashtable. The 'initsize' is the initial size of the table.
** When the load (the number of keys / the size), of the table reaches
** 'maxload', the table grows so that the load will become 'minload'.
** Default values for 'minload' and 'maxload' are 0.5 and 0.8 respectively.
** 'hfun' is the string hashfunction to use (default is hash_string_fast).
** 'dfun' is the optional destructor function for value data. It's called
** for values that are removed from the table, and when the table is
** destroyed.
*/
hashtable_t
hashtable_create(size_t initsize, float minload, float maxload,
		 hashfunc_t *hfun,
		 hashdestfunc_t *dfun);

/* Destroys a hashtable. If the table was created with a destructor function,
** it will be called for each value in the table.
*/
void
hashtable_destroy(hashtable_t h);

/* Puts the key-value pair into the table.
** Returns -1 on failure.
** Returns  0 on success, and if key didn't exist.
** Returns  1 on success, and if key was replaced.
*/
int
hashtable_put(hashtable_t h, const char *key, void *value);

/* Looks up the value for 'key' in the table. '*valuep' is updated
** unless 'valuep' is NULL.
** Returns -1 if not found
** Returns  0 if found, and '*valuep' updated to value.
*/
int
hashtable_get(hashtable_t h, const char *key, void **valuep);

/* Removes the 'key' (and its value of course) from the table.
** If 'valuep' is not NULL, '*valuep' is set to the value and
** the destructor is not called even if there is one.
** If 'valuep' is NULL, the destructor is called if the table
** was created with one.
** Returns -1 if not found
** Returns  0 if removed
*/
int
hashtable_rem(hashtable_t h, const char *key, void **valuep);

/* Returns some info about a hashtable.
** Each pointer will be set if it's non-NULL.
** '*sizep' is set to the size of the table.
** '*countp' is set to the number of key-value pairs in the table.
** '*slotsp' is set to the number of slots used in the table.
** '*cmaxp' is set to the length of the longest collision chain in the table.
** Access to the first two is instantaneous. To get the '*slotsp' value,
** the table is scanned once. To get the '*cmaxp', all the collision chains
** are scanned once too.
**
** The current load of the table is:      *countp / *sizep
** The average collision chain length is: *countp / *slotsp
** The number of collisions is:           *countp - *slotsp
*/
void
hashtable_info(hashtable_t h,
	       size_t *sizep, size_t *countp, size_t *slotsp, size_t *cmaxp);

#endif /* _hashtable_h_ */
