/* wstats.c
**
** Per-Erik Martin (per-erik.martin@telia.com) 2001-05-12
**
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define BUFSIZE 128

int
main()
{
  char buf[BUFSIZE];
  size_t lensum = 0;
  size_t words = 0;
  size_t minlen = 10000;
  size_t maxlen = 0;
  size_t ucase = 0;
  size_t lcase = 0;
  size_t nonascii = 0;
  size_t abelow = 0;

  while (fgets(buf, BUFSIZE, stdin))
  {
    size_t len = strlen(buf);

    if (len > 0 && buf[len-1] == '\n')
      buf[--len] = '\0';
    if (len > 0)
    {
      words += 1;
      lensum += len;
      if (len > maxlen)
	maxlen = len;
      if (len < minlen)
	minlen = len;
      while (len--)
      {
	int c = ((unsigned char *)buf)[len];

	if ('A' <= c && c <= 'Z')
	  ucase += 1;
	if ('a' <= c && c <= 'z')
	  lcase += 1;
	if (c > 127)
	  nonascii += 1;
	if (c < 'A')
	  abelow += 1;
      }
    }
  }

  printf("Words:          %6lu\n"
	 "Average length:    %6.2f\n"
	 "Min. length:    %6lu\n"
	 "Max. length:    %6lu\n"
	 "ASCII ucase:       %6.2f %%\n"
	 "ASCII lcase:       %6.2f %%\n"
	 "Non ASCII:         %6.2f %%\n"
	 "Below A:           %6.2f %%\n",
	 (unsigned long)words,
	 ((double)lensum)/words,
	 (unsigned long)minlen,
	 (unsigned long)maxlen,
	 (((double)ucase)/lensum) * 100,
	 (((double)lcase)/lensum) * 100,
	 (((double)nonascii)/lensum) * 100,
	 (((double)abelow)/lensum) * 100);

  exit(0);
}
