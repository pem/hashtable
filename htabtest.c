/* htabtest.c
**
** Per-Erik Martin (pem@pem.nu) 2001-05-13, 2009-08-12
**
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "hashtable.h"

int
main(int argc, char **argv)
{
  extern char *strdup(char *);
  size_t i, count;
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
  for (i = 0 ; i < count ; i++)
  {
      switch (hashtable_put(h, a[i], (void *)i))
      {
      case -1:
	fprintf(stderr, "hashtable_put(h, \"%s\", %lu) failed\n",
		a[i], (unsigned long)i);
	exit(1);
      case 1:
	printf("PUT: Replaced key: \"%s\" - %lu\n",
	       a[i], (unsigned long)i);
	break;
      }
  }

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
  while (i--)
  {
    size_t val;

    if (hashtable_get(h, a[i], (void **)&val) < 0)
      printf("GET: No \"%s\" found\n", a[i]);
  }

  i = count;
  while (i--)
  {
    if (hashtable_rem(h, a[i], NULL) < 0)
      printf("REM: No \"%s\" found\n", a[i]);
  }

  hashtable_destroy(h);
  for (i = 0 ; i < count ; i++)
      free(a[i]);
  free(a);

  exit(0);
}
