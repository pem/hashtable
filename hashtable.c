/* hashtable.c
**
** pem 2001-05-13, 2009-08-12
**
*/

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "hashtable.h"

#ifndef USE_MACROS
#define USE_MACROS 1
#endif

#define SWAP(A, B, TMP) ((TMP) = (A), (A) = (B), (B) = (TMP))

/*
** A key string type that avoids allocating small chunks
*/

#define HKEY_SHORT 7

typedef struct hkey_s
{
  char str[HKEY_SHORT+1];
  char *strp;
} hkey_t;

/* Returns strcmp style values, e.g. -1, 0, 1 */
static int
hkey_comp(hkey_t *hkeyp, const char *s)
{
  if (hkeyp->strp)
    return strcmp(hkeyp->strp, s);
  else
    return strcmp(hkeyp->str, s);
}

#if USE_MACROS
#define hkey_is_set(HP) ((HP)->strp != NULL || (HP)->str[0] != '\0')
#else
static bool
hkey_is_set(hkey_t *hkeyp)
{
  return (hkeyp->strp != NULL || hkeyp->str[0] != '\0');
}
#endif

static bool
hkey_set(hkey_t *hkeyp, const char *s)
{
  bool ret = true;
  size_t len = strlen(s);

  if (len <= HKEY_SHORT)
  {
    strcpy(hkeyp->str, s);
    hkeyp->strp = NULL;
  }
  else
  {
    hkeyp->str[0] = '\0';
    hkeyp->strp = (char *)malloc(len+1);
    if (hkeyp->strp == NULL)
      ret = false;
    else
      strncpy(hkeyp->strp, s, len+1);
  }
  return ret;
}

#if USE_MACROS
#define hkey_key(HP) ((HP)->strp ? (HP)->strp : (HP)->str)
#else
static char *
hkey_key(hkey_t *hkeyp)
{
  return (hkeyp->strp ? hkeyp->strp : hkeyp->str);
}
#endif

static void
hkey_clear(hkey_t *hkeyp)
{
  if (hkeyp)
  {
    if (hkeyp->strp)
      free(hkeyp->strp);
    hkeyp->strp = NULL;
    hkeyp->str[0] = '\0';
  }
}


/*
** A hashed datum structure
*/

typedef struct datum_s
{
  hkey_t hkey;
  void *value;
  struct datum_s *next;
} datum_t;

#if USE_MACROS
#define datum_set_value(DP, V) ((DP)->value = (V))
#define datum_set_next(DP1, DP2) ((DP1)->next = (DP2))
#else
static void
datum_set_value(datum_t *dp, void *val)
{
  dp->value = val;
}

static void
datum_set_next(datum_t *dp1, datum_t *dp2)
{
  dp1->next = dp2;
}
#endif /* !USE_MACROS */

static bool
datum_set(datum_t *dp, const char *hkey, void *val, datum_t *nextp)
{
  hkey_clear(&dp->hkey);
  if (!hkey_set(&dp->hkey, hkey))
    return false;
  dp->value = val;
  dp->next = nextp;
  return true;
}

static datum_t *
datum_copy(datum_t *dp)
{
  datum_t *dp2 = malloc(sizeof(datum_t));

  if (dp2)
  {
    memset(&dp2->hkey, 0, sizeof(dp2->hkey));
    if (!datum_set(dp2, hkey_key(&dp->hkey), dp->value, dp->next))
    {
      free(dp2);
      dp2 = NULL;
    }
  }
  return dp2;
}

static void
datum_clear(datum_t *dp)
{
  if (dp)
  {
    hkey_clear(&dp->hkey);
    dp->value = NULL;
    dp->next = NULL;
  }
}

static void
datum_free(datum_t *dp)
{
  datum_clear(dp);
  free(dp);
}

#if USE_MACROS
#define datum_is_set(DP)  hkey_is_set(&(DP)->hkey)
#define datum_key(DP)     hkey_key(&(DP)->hkey)
#define datum_value(DP)   ((DP)->value)
#define datum_comp(DP, S) hkey_comp(&(DP)->hkey, (S))
#define datum_next(DP)    ((DP)->next)
#else
static bool
datum_is_set(datum_t *dp)
{
  return hkey_is_set(&dp->hkey);
}

static char *
datum_key(datum_t *dp)
{
  return hkey_key(&dp->hkey);
}

static void *
datum_value(datum_t *dp)
{
  return dp->value;
}

/* Returns strcmp style values, e.g. -1, 0, 1 */
static int
datum_comp(datum_t *dp, const char *s)
{
  return hkey_comp(&dp->hkey, s);
}

static datum_t *
datum_next(datum_t *dp)
{
  return dp->next;
}
#endif /* !USE_MACROS */


/*
** Two good string hash functions
*/

hashval_t
hash_string_fast(const char *s)
{
  hashval_t val = 0;

  while (*s)
    val += (val << 3) + *s++;
  return val;
}

#define SEED_MAX 2147483646

hashval_t
hash_string_good(const char *s)
{
  uint32_t i;
  hashval_t val;
  union
  {
      hashval_t ul;
      char s[sizeof(hashval_t)];
  } u;

  /* Pack the first word */
  i = 0;
  while (i < sizeof(u.s) && *s)
    u.s[i++] = *s++;
  while (i < sizeof(u.s))
    u.s[i++] = '\0';		/* Pad if necessary */
  if (u.ul > SEED_MAX)
    u.ul -= SEED_MAX;

  /* Inline coding a the minimal standard pseudo random number generator */
#define RAND_A        16807
#define RAND_M   2147483647
#define RAND_Q       127773	/* m div a */
#define RAND_R         2836	/* m mod a */
  {
    register int32_t lo, hi, test;

    hi = u.ul/RAND_Q;
    lo = u.ul - hi*RAND_Q;	/* Seed mod RAND_Q */
    test = RAND_A*lo - RAND_R*hi;
    if (test <= 0)
      test += RAND_M;
    val = test;
  }
#undef RAND_A
#undef RAND_M
#undef RAND_Q
#undef RAND_R

  /* The rest is simply xor:ed wordwise */
  while (*s)
  {
    i = 0;
    while (i < sizeof(u.s) && *s)
      u.s[i++] = *s++;
    while (i < sizeof(u.s))
      u.s[i++] = '\0';
    val ^= u.ul;
  }

  return val;
}
#undef SEED_MAX

/*
** The hash table
*/

struct hashtable_s
{
  size_t size;
  size_t count;
  float minload;
  float maxload;
  hashfunc_t *hfun;		/* Hash function */
  hashdestfunc_t *dfun;		/* Destructor */
  datum_t *data;
};

hashtable_t
hashtable_create(size_t initsize, float minload, float maxload,
		 hashfunc_t *hfun,
		 hashdestfunc_t *dfun)
{
  hashtable_t table = malloc(sizeof(struct hashtable_s));

  if (table)
  {
    if (initsize == 0)
      initsize = 101;
    initsize |= 1;		/* Make it odd, it helps some hash functions */
    if (maxload < 0.5 || 1.0 <= maxload)
      maxload = 0.8;
    if (minload < 0.2 || maxload <= minload)
      minload = 0.5;
    if (minload >= maxload)
      minload = maxload / 2;
    table->size = initsize;
    table->count = 0;
    table->minload = minload;
    table->maxload = maxload;
    if (!hfun)
      hfun = hash_string_fast;
    table->hfun = hfun;
    table->dfun = dfun;
    table->data = malloc(initsize * sizeof(datum_t));
    if (table->data == NULL)
    {
      free(table);
      return NULL;
    }
    memset(table->data, 0, initsize * sizeof(datum_t));
  }
  return table;
}

void
hashtable_clear(hashtable_t h)
{
  for (size_t i = 0 ; i < h->size ; i++)
  {
    datum_t *dp = h->data + i;

    if (datum_is_set(dp))
    {
      void *val = datum_value(dp);
      datum_t *nextp = datum_next(dp);

      datum_clear(dp);
      if (h->dfun)
        h->dfun (val);
      dp = nextp;
      while (dp)
      {
        nextp = datum_next(dp);
        if (h->dfun)
          h->dfun (datum_value(dp));
        datum_free(dp);
        dp = nextp;
      }
    }
  }
  h->count = 0;
  memset(h->data, 0, h->size * sizeof(datum_t));
}

void
hashtable_destroy(hashtable_t h)
{
  hashtable_clear(h);
  free(h->data);
  free(h);
}

static hashtable_ret_t
hashtable_put_nogrow(hashtable_t h, const char *key, void *val, void **oldvalp);

/* Returns true on sucess
** Returns false on failure
*/
static bool
hashtable_grow(hashtable_t h)
{
  size_t newsize = (size_t) (h->count / h->minload);
  hashtable_t h2 = hashtable_create(newsize, h->minload, h->maxload,
				    h->hfun, NULL);

  if (h2)
  {
    size_t i;
    datum_t *data;

    for (i = 0 ; i < h->size ; i++)
    {
      datum_t *dp = h->data + i;

      if (datum_is_set(dp))
      {
	datum_t *nextp = datum_next(dp);

	if (hashtable_put_nogrow(h2, datum_key(dp), datum_value(dp), NULL) < 0)
	{
	  hashtable_destroy(h2);
	  return false;
	}
	dp = nextp;
	while (dp)
	{
	  nextp = datum_next(dp);
	  if (hashtable_put_nogrow(h2, datum_key(dp), datum_value(dp), NULL) < 0)
	  {
	    hashtable_destroy(h2);
	    return false;
	  }
	  dp = nextp;
	}
      }
    }
    SWAP(h->data, h2->data, data);
    SWAP(h->size, h2->size, i);
    SWAP(h->count, h2->count, i);
    hashtable_destroy(h2);
  }
  return true;
}

/* Returns true if found, and *dpp pointing to the entry, *prevp pointing to prev.
** Returns false if not found, and *dpp pointing the slot where it goes.
*/
static bool
hashtable_find(hashtable_t h, const char *key, datum_t **dpp, datum_t **prevp)
{
  datum_t *dp = h->data + (h->hfun(key) % h->size);

  if (datum_is_set(dp))
  {
    datum_t *p = dp;
    datum_t *prev = NULL;

    while (p)
    {
      if (datum_comp(p, key) == 0)
      {
	*dpp = p;
        if (prevp)
          *prevp = prev;
	return true;
      }
      prev = p;
      p = datum_next(p);
    }
  }
  *dpp = dp;
  return false;
}

/* Returns hashtable_ret_error on failure.
** Returns hashtable_ret_ok on success, and if key didn't exist.
** Returns hashtable_ret_replaced on success, and if key was replaced.
*/
static hashtable_ret_t
hashtable_put_nogrow(hashtable_t h, const char *key, void *val, void **oldvalp)
{
  datum_t *dp;

  if (hashtable_find(h, key, &dp, NULL))
  {				/* Found */
    if (oldvalp != NULL)
      *oldvalp = datum_value(dp); /* Return old one */
    else if (h->dfun)
      h->dfun (datum_value(dp)); /* Clear old one */
    if (!datum_set_value(dp, val))
        return hashtable_ret_error;
    return hashtable_ret_replaced;
  }
  else
  {				/* Not found */
    if (datum_is_set(dp))
    {				/* Push new value */
      datum_t *newp = datum_copy(dp); /* Copy the old one */

      if (!newp)
	return hashtable_ret_error;
      if (!datum_set(dp, key, val, newp)) /* Set the new one,    */
      {				          /* pointing to the old */
	datum_free(newp);
	return hashtable_ret_error;
      }
    }
    else
    {				/* Just smack it into this slot */
      if (!datum_set(dp, key, val, NULL))
	return hashtable_ret_error;
    }
    h->count += 1;
  }
   return hashtable_ret_ok;
}

/* Returns hashtable_ret_error on failure.
** Returns hashtable_ret_ok on success, and if key didn't exist.
** Returns hashtable_ret_replaced on success, and if key was replaced.
*/
hashtable_ret_t
hashtable_put(hashtable_t h, const char *key, void *val, void **oldvalp)
{
  if (key == NULL || key[0] == '\0')
    return hashtable_ret_error;
  if (((float)h->count+1) / h->size >= h->maxload)
  {
    if (!hashtable_grow(h))
      return hashtable_ret_error;
  }
  return hashtable_put_nogrow(h, key, val, oldvalp);
}

/* Returns hashtable_ret_not_found if not found
** Returns hashtable_ret_ok if found, and '*valuep' updated to value.
*/
hashtable_ret_t
hashtable_get(hashtable_t h, const char *key, void **valp)
{
  datum_t *dp;

  if (hashtable_find(h, key, &dp, NULL))
  {
    if (valp)
      *valp = datum_value(dp);
    return hashtable_ret_ok;
  }
  return hashtable_ret_not_found;
}

/* Returns hashtable_ret_not_found if not found
** Returns hashtable_ret_ok if removed
*/
hashtable_ret_t
hashtable_rem(hashtable_t h, const char *key, void **valp)
{
  datum_t *dp, *tmp;

  if (hashtable_find(h, key, &dp, &tmp))
  {
    if (valp)
      *valp = datum_value(dp);	/* Return old value */
    else if (h->dfun)
      h->dfun (datum_value(dp)); /* Destroy old value */
    if (!tmp)
    {                           /* No previous pointer */
      tmp = datum_next(dp);
      datum_clear(dp);
      if (tmp)
      {
        datum_set(dp, datum_key(tmp), datum_value(tmp), datum_next(tmp));
        datum_free(tmp);
      }
    }
    else
    {				/* Has a previous pointer */
      datum_set_next(tmp, datum_next(dp));
      datum_free(dp);
    }
    h->count -= 1;
    return hashtable_ret_ok;
  }
  return hashtable_ret_not_found;
}

void
hashtable_info(hashtable_t h,
	       size_t *sizep, size_t *countp, size_t *slotsp, size_t *cmaxp)
{
  if (sizep)
    *sizep = h->size;
  if (countp)
    *countp = h->count;
  if (slotsp || cmaxp)
  {
    size_t i, cmax = 0, scount = 0;

    for (i = 0 ; i < h->size ; i++)
    {
      datum_t *dp = h->data + i;

      if (datum_is_set(dp))
      {
	scount += 1;
	if (cmaxp)
	{
	  size_t c = 0;
	  while (dp)
	  {
	    c += 1;
	    dp = datum_next(dp);
	  }
	  if (c > cmax)
	    cmax = c;
	}
      }
    }
    if (slotsp)
      *slotsp = scount;
    if (cmaxp)
      *cmaxp = cmax;
  }
}

void
hashtable_iter_init(hashtable_t h, hashtable_iter_t *iterp)
{
  (void)h;			/* Unused */
  iterp->i = 0;
  iterp->p = NULL;
}

/* Returns true if a next value was found, with *keyp and *valuep
** updated, when non-NULL.
** Returns false when no more values are found.
*/
bool
hashtable_iter_next(hashtable_t h, hashtable_iter_t *iterp,
                    const char **keyp, void **valuep)
{
  datum_t *dp;

  if (iterp->p != NULL)
  {
    dp = (datum_t *)iterp->p;
    if (keyp != NULL)
      *keyp = datum_key(dp);
    if (valuep != NULL)
      *valuep = datum_value(dp);
    iterp->p = datum_next(dp);
    return true;
  }
  while (iterp->i < h->size)
  {
    dp = h->data + (iterp->i)++;
    if (! datum_is_set(dp))
      continue;
    if (keyp != NULL)
      *keyp = datum_key(dp);
    if (valuep != NULL)
      *valuep = datum_value(dp);
    iterp->p = datum_next(dp);
    return true;
  }
  return false;
}
