#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "hTree.h"

#define MAX_KEYS (1024)

struct htNode {
    int isLeaf;     /* is this a leaf node? */
    int numKeys;    /* how many keys does this node contain? */
    int keys[MAX_KEYS];
    struct htNode *kids[MAX_KEYS+1];  /* kids[i] holds nodes < keys[i] */
};

hTree
htCreate(void)
{
    hTree h;

    h = malloc(sizeof(*h));
    assert(h);

    h->isLeaf = 1;
    h->numKeys = 0;

    return h;
}

void
htDestroy(hTree h)
{
    int i;

    if(!h->isLeaf) {
        for(i = 0; i < h->numKeys + 1; i++) {
            htDestroy(h->kids[i]);
        }
    }

    free(h);
}

/* return smallest index i in sorted array such that key <= a[i] */
/* (or n if there is no such index) */
static int
searchKey(int n, const int *a, int key)
{
    int lo;
    int hi;
    int mid;

    /* invariant: a[lo] < key <= a[hi] */
    lo = -1;
    hi = n;

    while(lo + 1 < hi) {
        mid = (lo+hi)/2;
        if(a[mid] == key) {
            return mid;
        } else if(a[mid] < key) {
            lo = mid;
        } else {
            hi = mid;
        }
    }

    return hi;
}

int
htSearch(hTree h, int key)
{
    int pos;

    /* have to check for empty tree */
    if(h->numKeys == 0) {
        return 0;
    }

    /* look for smallest position that key fits below */
    pos = searchKey(h->numKeys, h->keys, key);

    if(pos < h->numKeys && h->keys[pos] == key) {
        return 1;
    } else {
        return(!h->isLeaf && htSearch(h->kids[pos], key));
    }
}

/* insert a new key into a tree */
/* returns new right sibling if the node splits */
/* and puts the median in *median */
/* else returns 0 */
static hTree
htInsertInternal(hTree h, int key, int *median)
{
    int pos;
    int mid;
    hTree h2;

    pos = searchKey(h->numKeys, h->keys, key);

    if(pos < h->numKeys && h->keys[pos] == key) {
        /* nothing to do */
        return 0;
    }

    if(h->isLeaf) {

        /* everybody above pos moves up one space */
        memmove(&h->keys[pos+1], &h->keys[pos], sizeof(*(h->keys)) * (h->numKeys - pos));
        h->keys[pos] = key;
        h->numKeys++;

    } else {

        /* insert in child */
        h2 = htInsertInternal(h->kids[pos], key, &mid);
        
        /* maybe insert a new key in b */
        if(h2) {

            /* every key above pos moves up one space */
            memmove(&h->keys[pos+1], &h->keys[pos], sizeof(*(h->keys)) * (h->numKeys - pos));
            /* new kid goes in pos + 1*/
            memmove(&h->kids[pos+2], &h->kids[pos+1], sizeof(*(h->keys)) * (h->numKeys - pos));

            h->keys[pos] = mid;
            h->kids[pos+1] = h2;
            h->numKeys++;
        }
    }

    /* we waste a tiny bit of space by splitting now
     * instead of on next insert */
    if(h->numKeys >= MAX_KEYS) {
        mid = h->numKeys/2;

        *median = h->keys[mid];

        /* make a new node for keys > median */
        /* picture is:
         *
         *      3 5 7
         *      A B C D
         *
         * becomes
         *          (5)
         *      3        7
         *      A B      C D
         */
        h2 = malloc(sizeof(*h2));

        h2->numKeys = h->numKeys - mid - 1;
        h2->isLeaf = h->isLeaf;

        memmove(h2->keys, &h->keys[mid+1], sizeof(*(h->keys)) * h2->numKeys);
        if(!h->isLeaf) {
            memmove(h2->kids, &h->kids[mid+1], sizeof(*(h->kids)) * (h2->numKeys + 1));
        }

        h->numKeys = mid;

        return h2;
    } else {
        return 0;
    }
}

void
htInsert(hTree h, int key)
{
    hTree h1;   /* new left child */
    hTree h2;   /* new right child */
    int median;

    h2 = htInsertInternal(h, key, &median);

    if(h2) {
        /* basic issue here is that we are at the root */
        /* so if we split, we have to make a new root */

        h1 = malloc(sizeof(*h1));
        assert(h1);

        /* copy root to h1 */
        memmove(h1, h, sizeof(*h));

        /* make root point to h1 and h2 */
        h->numKeys = 1;
        h->isLeaf = 0;
        h->keys[0] = median;
        h->kids[0] = h1;
        h->kids[1] = h2;
    }
}