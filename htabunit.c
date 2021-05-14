/*
** pem 2021-05-14
**
** A simple unit test for the hashtables.
*/

#include <stdlib.h>
#include <stdio.h>
#include "hashtable.h"

static char *Words[] =
    {
     "first", "second", "third", "fourth", "fifth",
     "sixth", "seventh", "eigth", "ninth", "tentn",
     NULL
    };

static void
print_info(hashtable_t h)
{
    size_t size, count, slots, cmax;

    hashtable_info(h, &size, &count, &slots, &cmax);
    printf("    Size: %2lu  Count: %2lu  Slots: %2lu  Chain max.: %2lu"
           "  Load: %4.2f\n",
           (unsigned long)size, (unsigned long)count,
           (unsigned long)slots, (unsigned long)cmax,
           ((float)count) / size);
}

int
main()
{
    int i;
    hashtable_t h;

    h = hashtable_create(10, 0.5, 0.8, hash_string_fast, NULL);
    if (h == NULL)
    {
        fprintf(stderr, "Failed to create hash table\n");
        exit(1);
    }
    putchar('\n');
    printf("### New table, FAST hash function, min load 0.5, max load 0.8\n");
    print_info(h);
    for (i = 0 ; i < 8 && Words[i] != NULL ; i++)
        if (hashtable_put(h, Words[i], Words[i], NULL) < 0)
        {
            fprintf(stderr, "Failed to put key %s\n", Words[i]);
            exit(1);
        }
    printf("### %d words in table\n", i);
    print_info(h);
    if (hashtable_put(h, Words[i], Words[i], NULL) < 0)
    {
        fprintf(stderr, "Failed to put key %s\n", Words[i]);
        exit(1);
    }
    printf("### 11 words in table\n");
    print_info(h);
    putchar('\n');

    hashtable_destroy(h);

    h = hashtable_create(10, 0.2, 0.9, hash_string_good, NULL);
    if (h == NULL)
    {
        fprintf(stderr, "Failed to create hash table\n");
        exit(1);
    }
    printf("### New table, GOOD hash function, min load 0.2, max load 0.9\n");
    print_info(h);
    for (i = 0 ; i < 9 && Words[i] != NULL ; i++)
        if (hashtable_put(h, Words[i], Words[i], NULL) < 0)
        {
            fprintf(stderr, "Failed to put key %s\n", Words[i]);
            exit(1);
        }
    printf("### %d words in table\n", i);
    print_info(h);
    if (hashtable_put(h, Words[i], Words[i], NULL) < 0)
    {
        fprintf(stderr, "Failed to put key %s\n", Words[i]);
        exit(1);
    }
    printf("### %d words in table\n", i);
    print_info(h);
    putchar('\n');

    hashtable_clear(h);
    printf("### Cleared table\n");
    print_info(h);
    putchar('\n');

    hashtable_destroy(h);
    exit(0);
}
