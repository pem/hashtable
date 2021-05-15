/*
** pem 2021-05-14
**
** A simple unit test for the hashtables.
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
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

static void
perrex(const char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    exit(1);
}

int
main()
{
    int i;
    char *val;
    hashtable_t h;

    /*
    ** Small table, default values, fast, test grow
    */
    h = hashtable_create(10, 0.5, 0.8, hash_string_fast, NULL);
    if (h == NULL)
        perrex("Failed to create hash table\n");
    putchar('\n');
    printf("### New table, FAST hash function, min load 0.5, max load 0.8\n");
    print_info(h);
    for (i = 0 ; i < 8 && Words[i] != NULL ; i++)
        if (hashtable_put(h, Words[i], Words[i], NULL) != hashtable_ret_ok)
            perrex("Failed to put key %s\n", Words[i]);
    printf("### %d words in table\n", i);
    print_info(h);
    if (hashtable_put(h, Words[i], Words[i], NULL) != hashtable_ret_ok)
        perrex("Failed to put key %s\n", Words[i]);
    i += 1;
    printf("### %d words in table\n", i);
    print_info(h);
    putchar('\n');

    /*
    ** Don't find non-existing key
    */
    if (hashtable_get(h, "xyz", (void **)&val) != hashtable_ret_not_found)
        perrex("Found xyz:%s in table, shouln't have.\n", val);
    printf("### Get not found test ok\n" );
    putchar('\n');
    if (hashtable_rem(h, "abc", (void **)&val) != hashtable_ret_not_found)
        perrex("Removed abc:%s in table, shouln't have.\n", val);
    printf("### Remove not found test ok\n");
    putchar('\n');

    /*
    ** Remove them one-by-one
    */
    for (int j = 0 ; j < i && Words[j] != NULL ; j++)
    {
        if (hashtable_rem(h, Words[j], (void **)&val) != hashtable_ret_ok)
            perrex("Failed to remove key %s\n", Words[j]);
        if (strcmp(Words[j], val) != 0)
            perrex("Removed value mismatch: %s != %s\n", Words[j], val);
    }
    printf("### All keys removed\n");
    print_info(h);
    putchar('\n');

    hashtable_destroy(h);

    /*
    ** Small table, custom values, "good", test grow
    */
    h = hashtable_create(10, 0.2, 0.9, hash_string_good, NULL);
    if (h == NULL)
        perrex("Failed to create hash table\n");
    printf("### New table, GOOD hash function, min load 0.2, max load 0.9\n");
    print_info(h);
    for (i = 0 ; i < 9 && Words[i] != NULL ; i++)
        if (hashtable_put(h, Words[i], Words[i], NULL) != hashtable_ret_ok)
            perrex("Failed to put key %s\n", Words[i]);
    printf("### %d words in table\n", i);
    print_info(h);
    if (hashtable_put(h, Words[i], Words[i], NULL) != hashtable_ret_ok)
        perrex("Failed to put key %s\n", Words[i]);
    i += 1;
    printf("### %d words in table\n", i);
    print_info(h);
    putchar('\n');

    /*
    ** Find them all
    */
    for (int j = 0 ; j < i && Words[j] != NULL ; j++)
    {
        if (hashtable_get(h, Words[j], (void **)&val) != hashtable_ret_ok)
            perrex("Failed to get key %s\n", Words[j]);
    }
    printf("### Found all keys\n");
    putchar('\n');

    /*
    ** Iterate
    */
    int count = 0;
    const char *key;
    hashtable_iter_t iter;
    hashtable_iter_init(h, &iter);
    while (hashtable_iter_next(h, &iter, &key, (void **)&val))
    {
        count += 1;
        if (strcmp(key, val) != 0)
            perrex("Key-val mismatch: %s != %s\n", key, val);
    }
    if (i != count)
        perrex("Iterator found %d keys, expected %d\n", count, i);
    printf("### Iterated over all keys\n");
    putchar('\n');

    /*
    ** Clear the table
    */
    hashtable_clear(h);
    printf("### Cleared table\n");
    print_info(h);
    putchar('\n');

    hashtable_destroy(h);

    /*
    ** Default table with deallocator
    */
    h = hashtable_create_dest_default(free);
    if (h == NULL)
        perrex("Failed to create hash table\n");
    printf("### New table, default with deallocator\n");
    print_info(h);
    for (i = 0 ; Words[i] != NULL ; i++)
        if (hashtable_put(h, Words[i], strdup(Words[i]), NULL) != hashtable_ret_ok)
            perrex("Failed to put key %s\n", Words[i]);
    printf("### %d words in table\n", i);
    print_info(h);
    putchar('\n');

    /*
    ** Replace a value
    */
    if (hashtable_put(h, Words[0], strdup("123456"), (void **)&val) != hashtable_ret_replaced)
        perrex("Failed to replace value for %s\n", Words[0]);
    if (strcmp(val, Words[0]) != 0)
        perrex("Old value didn't match: %s != %s\n", val, Words[0]);
    free(val);
    if (hashtable_get(h, Words[0], (void **)&val) != hashtable_ret_ok)
        perrex("Failed to get %s\n", Words[0]);
    if (strcmp(val, "123456") != 0)
        perrex("New value does not match 123456: %s\n", val);
    /* Replace again (old value should be freed) */
    if (hashtable_put(h, Words[0], strdup("ABCDEF"), NULL) != hashtable_ret_replaced)
        perrex("Failed to replace value for %s\n", Words[0]);
    printf("### Replace ok\n");
    print_info(h);
    putchar('\n');

    /*
    ** Remove them one-by-one without returning the value.
    ** (They should be freed.)
    */
    for (i = 0 ; Words[i] != NULL ; i++)
    {
        if (hashtable_rem(h, Words[i], NULL) != hashtable_ret_ok)
            perrex("Failed to remove key %s\n", Words[i]);
    }
    printf("### All keys removed\n");
    print_info(h);
    putchar('\n');

    hashtable_destroy(h);

    printf("Ok\n");

    exit(0);
}
