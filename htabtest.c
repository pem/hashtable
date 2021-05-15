/* htabtest.c
**
** pem 2001-05-13, 2009-08-12, 2021-05-14
**
** Reads lines from standar input and puts them into a an array.
** The times the time it takes to put them into a hashtable,
** lookup each one, and then remove them all, from the table.
** Also prints some statistics about the table.
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>

#include "hashtable.h"

#define STV2DOUBLE(T) ((T)->tv_sec + (T)->tv_usec/1000000.0)
#define STVDIFF(T1, T0) (STV2DOUBLE(T1) - STV2DOUBLE(T0))

static void
print_time(const char *s, struct timeval *t0, struct timeval *t1)
{
    printf("%s %4.3f ms\n", s, 1000*STVDIFF(t1, t0));
}

int
main(int argc, char **argv)
{
  size_t i, count;
  struct timeval t0, t1;
  char buf[128];
  size_t size = 0;
  char **a = NULL;
  hashtable_t h;
  hashfunc_t *hfun = hash_string_fast;

  if (argc == 2 && strcmp(argv[1], "-g") == 0)
    hfun = hash_string_good;

  count = 0;
  while (fgets(buf, sizeof(buf), stdin))
  {
    size_t len = strlen(buf);

    if (len > 0)
    {
      if (buf[len-1] == '\n')
	buf[--len] = '\0';
    }
    if (count >= size)
    {
      size += 100;
      a = realloc(a, size * sizeof(char *));
      if (!a)
      {
	fprintf(stderr, "realloc(a, %lu) failed\n",
		(unsigned long)size * sizeof(char *));
	exit(1);
      }
    }
    a[count] = strdup(buf);
    if (!a[count])
    {
      fprintf(stderr, "strdup(\"%s\") failed\n", buf);
      exit(1);
    }
    count += 1;
  }

  h = hashtable_create(count, 0.5, 0.8,
		       hfun, NULL);

  if (!h)
  {
    fprintf(stderr, "hashtable_create() failed\n");
    exit(1);
  }
  gettimeofday(&t0, NULL);
  for (i = 0 ; i < count ; i++)
  {
      switch (hashtable_put(h, a[i], (void *)i, NULL))
      {
      case hashtable_ret_error:
	fprintf(stderr, "hashtable_put(h, \"%s\", %lu) failed\n",
		a[i], (unsigned long)i);
	exit(1);
      case hashtable_ret_replaced:
	printf("PUT: Replaced key: \"%s\" - %lu\n",
	       a[i], (unsigned long)i);
	break;
      default:
        break;
      }
  }
  gettimeofday(&t1, NULL);
  print_time("Insert:", &t0, &t1);

  {
    size_t isize, icount, islots, icmax;

    hashtable_info(h, &isize, &icount, &islots, &icmax);

    printf("Size:       %6lu\n"
	   "Count:      %6lu\n"
	   "Slots:      %6lu\n"
	   "Chain max:  %6lu\n"
	   "Load:            %1.2f\n"
	   "Av. chain:       %1.2f\n"
	   "Collisions: %6lu (%.2f %%)\n",
	   (unsigned long)isize,
	   (unsigned long)icount,
	   (unsigned long)islots,
	   (unsigned long)icmax,
	   ((double)icount)/isize,
	   ((double)icount)/islots,
	   (unsigned long)icount - islots,
	   ((double)(icount-islots)/icount)*100.0);
  }

  if (0)
  {
      hashtable_iter_t iter;
      char *key;
  
      hashtable_iter_init(h, &iter);
      while (hashtable_iter_next(h, &iter, (void *)&key, NULL))
          printf("%s\n", key);
  }

  i = count;
  gettimeofday(&t0, NULL);
  while (i--)
  {
    size_t val;

    if (hashtable_get(h, a[i], (void **)&val) == hashtable_ret_not_found)
      printf("GET: No \"%s\" found\n", a[i]);
  }
  gettimeofday(&t1, NULL);
  print_time("Find:  ", &t0, &t1);

  i = count;
  gettimeofday(&t0, NULL);
  while (i--)
  {
    if (hashtable_rem(h, a[i], NULL) == hashtable_ret_not_found)
      printf("REM: No \"%s\" found\n", a[i]);
  }
  gettimeofday(&t1, NULL);
  print_time("Delete:", &t0, &t1);

  hashtable_destroy(h);
  for (i = 0 ; i < count ; i++)
      free(a[i]);
  free(a);

  exit(0);
}
