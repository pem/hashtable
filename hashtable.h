/* hashtable.h
**
** pem  2001-05-13, 2009-08-12
**
*/

#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct hashtable_s *hashtable_t;

typedef struct hashtable_iter_s
{
    size_t i;
    void *p;
} hashtable_iter_t;

typedef uint32_t hashval_t;

typedef enum hashtable_ret_e
  {
   hashtable_ret_error,         /* Probably out of memory */
   hashtable_ret_ok,
   hashtable_ret_replaced,
   hashtable_ret_not_found
  } hashtable_ret_t;

/* The type for a string hash function */
typedef hashval_t
hashfunc_t(const char *s);

/* A type for a destructor function for the value data */
typedef void
hashdestfunc_t(void *);

/* This is a reasonably good, and fast string hash function */
extern hashval_t
hash_string_fast(const char *s);

/* This one is very good, esp. on difficults sets of keys, but
** about twice as slow as hash_string_fast()
*/
extern hashval_t
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
extern hashtable_t
hashtable_create(size_t initsize, float minload, float maxload,
		 hashfunc_t *hfun,
		 hashdestfunc_t *dfun);
/* Create with just default values */
#define hashtable_create_default() hashtable_create(0, 0, 0, NULL, NULL)
/* Create with default values and a destructor */
#define hashtable_create_dest_default(D) hashtable_create(0, 0, 0, NULL, (D))

/* Clears a hashtable, but keeps the table object itself with its initial
** settings. If the table was created with a destructor function,
** it will be called for each value in the table.
*/
extern void
hashtable_clear(hashtable_t h);

/* Destroys a hashtable. If the table was created with a destructor function,
** it will be called for each value in the table.
*/
extern void
hashtable_destroy(hashtable_t h);

/* Puts the key-value pair into the table. Note that 'key' must not be
** NULL or an empty string. If 'oldvalp' != NULL, it's set to the old
** value if it existed.
** Returns hashtable_ret_error on failure.
** Returns hashtable_ret_ok on success, and if key didn't exist.
** Returns hashtable_ret_replaced on success, and if key was replaced.
*/
hashtable_ret_t
hashtable_put(hashtable_t h, const char *key, void *val, void **oldvalp);

/* Looks up the value for 'key' in the table. '*valuep' is updated
** unless 'valuep' is NULL.
** Returns hashtable_ret_not_found if not found
** Returns hashtable_ret_ok if found, and '*valuep' updated to value.
*/
hashtable_ret_t
hashtable_get(hashtable_t h, const char *key, void **valuep);

/* Removes the 'key' (and its value of course) from the table.
** If 'valuep' is not NULL, '*valuep' is set to the value and
** the destructor is not called even if there is one.
** If 'valuep' is NULL, the destructor is called if the table
** was created with one.
** Returns hashtable_ret_not_found if not found
** Returns hashtable_ret_ok if removed
*/
hashtable_ret_t
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
extern void
hashtable_info(hashtable_t h,
	       size_t *sizep, size_t *countp, size_t *slotsp, size_t *cmaxp);

/* Initialize an iterator.
** WARNING: Do not add or delete anything from a hashtable while an
**          iterator is in use!
*/
extern void
hashtable_iter_init(hashtable_t h, hashtable_iter_t *iterp);

/* Gets the next key and value from a table and iterator. 'keyp' or
** 'valuep' may be NULL.
** Returns true when there was a next value, false when the end of the table
** was reached. Values are returned in some arbitrary order.
** WARNING: Do not add or delete anything from a hashtable while an
**          iterator is in use!
*/
extern bool
hashtable_iter_next(hashtable_t h, hashtable_iter_t *iterp,
                    const char **keyp, void **valuep);
