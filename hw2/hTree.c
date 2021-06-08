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
    ht->h = hash_init();

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
struct dir_entry
htSearch(const struct hTree h, const char *name,
        struct dir_entry *ep, off_t *ofsp)
{
    //TODO
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