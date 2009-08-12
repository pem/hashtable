/* hash-string.c
**
** Per-Erik Martin (per-erik.martin@telia.com) 2001-05-11
**
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <limits.h>
#include <stdint.h>

#define BUFSIZE   128

#if 0
/*
** From SWI-Prolog, pl-3.4.5/src/pl-alloc.c
*/

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Hash function for strings.  This function has been evaluated on Shelley,
which defines about 5000 Prolog atoms.  It produces a very nice  uniform
distribution over these atoms.  Note that size equals 2^n.
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

int
unboundStringHashValue(const char *t, unsigned int len)
{ unsigned int value = 0;
  unsigned int shift = 5;

  while(len-- != 0)
  { unsigned int c = *t++;
    
    c -= 'a';
    value ^= c << (shift & 0xf);
    shift ^= c;
  }

  return value ^ (value >> 16);
}

/*
** From CMULISP, cmucl-18c/src/lisp/vars.c
*/

static int hash_name(char *name)
{
    unsigned long value = 0;

    while (*name != '\0') {
        value = (value << 1) ^ *(unsigned char *)(name++);
        value = (value & (1-(1<<24))) ^ (value >> 24);
    }

    return value % NAME_BUCKETS;
}

/* From genlib/lib/hash.c */

/*
 * Compare/Hash function, based on string hash function from Tcl 7.3.
 */
static int
cmphash(char *str, char *str2)
{
    unsigned char *s;
    int val;
    
    if (str2 == NULL) {
        val = 0;
        for (s = (unsigned char *)str; *s; s++) {
            val += (val << 3) + *s;
        }
        return (val > 0) ? val : -val;
    }
    return (str == str2) ? 0 : strcmp(str, str2);
}
#endif /* 0 */

unsigned long
hash_string1(const char *s)
{
  unsigned long value = 0;
  unsigned long shift = 5;

  while(*s)
  {
    unsigned c = *s++;
    
    c -= 'a';
    value ^= c << (shift & 0xf);
    shift ^= c;
  }

  return (value ^ (value >> 16));
}

unsigned long
hash_string2(const char *s)
{
  unsigned long value = 0;

  while (*s)
  {
    value = (value << 1) ^ *(unsigned char *)(s++);
    value = (value & (1-(1<<24))) ^ (value >> 24);
  }

  return value;
}

unsigned long
hash_string3(const char *s)
{
  unsigned long val = 0;

  while (*s)
    val += (val << 3) + *s++;
  return val;
}

#define SEED_MAX 2147483646L
#define RAND_A      16807L
#define RAND_M 2147483647L
#define RAND_Q     127773L	/* m div a */
#define RAND_R       2836L	/* m mod a */

static unsigned long
hashrand(unsigned long seed)
{
  register long lo, hi, test;

  hi = seed/RAND_Q;
  lo = seed - hi*RAND_Q;		/* Seed mod RAND_Q */
  test = RAND_A*lo - RAND_R*hi;
  if (test <= 0)
    test += RAND_M;
  return test;
}
#undef RAND_A
#undef RAND_M
#undef RAND_Q
#undef RAND_R

unsigned long
hash_string4(const char *s)
{
  unsigned long val = 0;
  union
  {
      unsigned long ul;
      char s[sizeof(unsigned long)];
  } u;

  while (*s)
  {
    unsigned i = sizeof(unsigned long);

    while (i && *s)
      u.s[--i] = *s++;
    while (i)
      u.s[--i] = '\0';
    if (u.ul > SEED_MAX)
      u.ul -= SEED_MAX;
    val ^= hashrand(u.ul);
  }
  return val;
}


/*
** hash_string5
*/

typedef unsigned int shidx_t;

static shidx_t T[] =
{
  1, 87, 49, 12, 176, 178, 102, 166, 121, 193, 6, 84, 249, 230, 44, 163,
  14, 197, 213, 181, 161, 85, 218, 80, 64, 239, 24, 226, 236, 142, 38, 200,
  110, 177, 104, 103, 141, 253, 255, 50, 77, 101, 81, 18, 45, 96, 31, 222,
  25, 107, 190, 70, 86, 237, 240, 34, 72, 242, 20, 214, 244, 227, 149, 235,
  97, 234, 57, 22, 60, 250, 82, 175, 208, 5, 127, 199, 111, 62, 135, 248,
  174, 169, 211, 58, 66, 154, 106, 195, 245, 171, 17, 187, 182, 179, 0, 243,
  132, 56, 148, 75, 128, 133, 158, 100, 130, 126, 91, 13, 153, 246, 216, 219,
  119, 68, 223, 78, 83, 88, 201, 99, 122, 11, 92, 32, 136, 114, 52, 10,
  138, 30, 48, 183, 156, 35, 61, 26, 143, 74, 251, 94, 129, 162, 63, 152,
  170, 7, 115, 167, 241, 206, 3, 150, 55, 59, 151, 220, 90, 53, 23, 131,
  125, 173, 15, 238, 79, 95, 89, 16, 105, 137, 225, 224, 217, 160, 37, 123,
  118, 73, 2, 157, 46, 116, 9, 145, 134, 228, 207, 212, 202, 215, 69, 229,
  27, 188, 67, 124, 168, 252, 42, 4, 29, 108, 21, 247, 19, 205, 39, 203,
  233, 40, 186, 147, 198, 192, 155, 33, 164, 191, 98, 204, 165, 180, 117, 76,
  140, 36, 210, 172, 41, 54, 159, 8, 185, 232, 113, 196, 231, 47, 146, 120,
  51, 65, 28, 144, 254, 221, 93, 189, 194, 139, 112, 43, 71, 109, 184, 209
};

static shidx_t
H(const unsigned char *str)
{
  register shidx_t h = 0;

  while (*str)
    h = T[h ^ *str++];
  return h;
}

static shidx_t
H2(unsigned char first, const unsigned char *rest)
{
  register shidx_t h = T[(int)first];

  while (*rest)
    h = T[h ^ *rest++];
  return h;
}

/* Was shidx_t H16(char *str) */
unsigned long
hash_string5(const char *str)
{
  shidx_t h = H((unsigned char *)str);

  h = ((h << CHAR_BIT) | H2(((unsigned char)str[0]+1) % 256,
			    (unsigned char *)str+1));
  return (unsigned long)h;
}

/*
** End of hash_string5
*/
#if 0
unsigned long
hash_string6(const char *s)
{
  unsigned long w = 0;
  unsigned long val = 0;

  while (*s)
  {
    unsigned char c = *(unsigned char *)(s++);
    val ^= w ^ c;
    w = (w << 4) ^ c;
  }
  return val;
}
#else
unsigned long
hash_string6(const char *s)
{
  register unsigned i;
  unsigned long val;
  union
  {
      unsigned long ul;
      char s[sizeof(unsigned long)];
  } u;

  i = sizeof(unsigned long);

  while (i && *s)
    u.s[--i] = *s++;
  while (i)
    u.s[--i] = '\0';
  if (u.ul > SEED_MAX)
    u.ul -= SEED_MAX;

#define SEED_MAX 2147483646L
#define RAND_A      16807L
#define RAND_M 2147483647L
#define RAND_Q     127773L	/* m div a */
#define RAND_R       2836L	/* m mod a */
  {
    register long lo, hi, test;

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

  while (*s)
  {
    i = sizeof(unsigned long);
    while (i && *s)
      u.s[--i] = *s++;
    while (i)
      u.s[--i] = '\0';
    val ^= u.ul;
  }

  return val;
}
#endif

typedef unsigned long hashfun_t(const char *);

void
tryhash(char **words, size_t count,
	unsigned *a, size_t tsize,
	unsigned *s, size_t smax,
	hashfun_t *hfun, char *fname)
{
  size_t i, j, empty, used, min, max;
  time_t t0, t1;

  memset(a, 0, tsize * sizeof(unsigned));

  t0 = time(NULL);
  j = 20000000/count;		/* On a 450MHz PII */
  while (j--)
    for (i = 0 ; i < count ; i++)
      a[hfun(words[i]) % tsize] += 1;
  t1 = time(NULL);

  memset(a, 0, tsize * sizeof(unsigned));
  for (i = 0 ; i < count ; i++)
    a[hfun(words[i]) % tsize] += 1;

  memset(s, 0, smax * sizeof(unsigned));
  min = SIZE_MAX;
  max = 0;
  empty = used = 0;
  for (i = 0 ; i < tsize ; i++)
  {
    if (a[i])
      used += 1;
    else
      empty += 1;
    if (a[i] >= smax)
      s[smax-1] += 1;
    else
      s[a[i]] += 1;
    if (a[i] < min)
      min = a[i];
    if (a[i] > max)
      max = a[i];
  }

  printf("\nFunction: %s\n", fname);
  printf("Empty: %6lu\n", (unsigned long)empty);
  printf("Used:  %6lu\n", (unsigned long)used);
  printf("Min:   %6lu\n", (unsigned long)min);
  printf("Max:   %6lu\n", (unsigned long)max);
  printf("Aver.: %g\n", ((double)count)/used);
  printf("Time:  %g s\n", difftime(t1, t0));
}

int
main(int argc, char **argv)
{
  char **words = NULL;
  size_t size = 0;
  unsigned *a;
  unsigned *s;
  size_t count;
  size_t smax;
  size_t tsize = 0;
  char buf[BUFSIZE];

  if (argc == 2)
    tsize = (unsigned long)atoi(argv[1]);

  for (count = 0 ; fgets(buf, BUFSIZE, stdin) != NULL ; count++)
  {
    size_t len = strlen(buf);

    if (len > 0 && buf[len-1] == '\n')
      buf[--len] = '\0';
    if (len > 0)
    {
      if (count >= size)
      {
	size += 10000;
	words = realloc(words, size * sizeof(char *));
	if (words == NULL)
	{
	  fprintf(stderr, "realloc(...,%lu) failed\n",
		  (unsigned long)size * sizeof(char *));
	  exit(1);
	}
      }
      words[count] = strdup(buf);
      if (words[count] == NULL)
      {
	fprintf(stderr, "strdup(\"%s\") failed\n", buf);
	exit(1);
      }
    }
  }
  printf("Read: %lu words\n", (unsigned long)count);
  if (count == 0)
  {
    fprintf(stderr, "Need data to proceed\n");
    exit(1);
  }

  if (tsize == 0)
    tsize = count;

  printf("Table size: %lu\n", (unsigned long)tsize);

  a = malloc(tsize * sizeof(unsigned));
  if (a == NULL)
  {
    fprintf(stderr, "malloc(%lu) failed\n",
	    (unsigned long)tsize * sizeof(unsigned));
    exit(1);
  }

  smax = tsize/100;
  if (smax < 1)
    smax = 100;
  s = malloc(smax * sizeof(unsigned));
  if (s == NULL)
  {
    fprintf(stderr, "malloc(%lu) failed\n",
	    (unsigned long)smax * sizeof(unsigned));
    exit(1);
  }

#if 1
  tryhash(words, count, a, tsize, s, smax, hash_string1, "hash_string1");
  /*  tryhash(words, count, a, tsize, s, smax, hash_string2, "hash_string2");*/
  tryhash(words, count, a, tsize, s, smax, hash_string3, "hash_string3");
  tryhash(words, count, a, tsize, s, smax, hash_string4, "hash_string4");
  tryhash(words, count, a, tsize, s, smax, hash_string5, "hash_string5");
#endif
  tryhash(words, count, a, tsize, s, smax, hash_string6, "hash_string6");

  exit(0);
}
