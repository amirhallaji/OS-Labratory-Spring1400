#include <debug.h>
#include <string.h>

#include "threads/malloc.h"
#include "lib/kernel/hash.h"
#include "hTree.h"
#include "bTree.h"

#define MAX_KEYS (1024)

struct htNode {
    struct bTree b;
    struct hash h;
};

struct hTree
htCreate(void)
{
    hTree ht;

    ht->b = btCreate();
    hash_init(ht->h, dir_hash, dir_less, NULL);

    return ht;
}

void
htDestroy(hTree ht)
{
    btDestroy(ht->b);
    hash_clear(ht->h);
}

/* Searches DIR for a file with the given NAME.
   If successful, returns true, sets *EP to the directory entry
   if EP is non-null, and sets *OFSP to the byte offset of the
   directory entry if OFSP is non-null.
   otherwise, returns false and ignores EP and OFSP. */
struct bool
htSearch(const struct hTree ht, const char *name,
        struct dir_entry *ep, off_t *ofsp)
{
    int key;

    e = hash_find (ht->h, &name);
    if (e == NULL)
        return NULL
    key = hash_entry (e, struct dir_entry, char);
    btSearch(ht->b, key, &ep, &ofsp);
}

/* Insert a file named NAME to DIR. The file's inode is in sector
   INODE_SECTOR.
   Returns true if successful, false on failure.
   Fails if a disk or memory error occurs. */
void
htInsert(hTree h, int key, block_sector_t inode_sector,
        struct dir_entry *ep, off_t *ofsp)
{
    //TODO
}