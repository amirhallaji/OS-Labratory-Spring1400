/* implementation of a H-tree */
typedef struct htNode *hTree;

/* create a new empty tree */
hTree htCreate(void);

/* free a tree */
void htDestroy(hTree t);

/* return nonzero if key is present in tree */
int htSearch(const struct hTree t, const char *name, struct dir_entry *ep, off_t *ofsp);

/* insert a new element into a tree */
void htInsert(hTree h, int key, block_sector_t inode_sector, struct dir_entry *ep, off_t *ofsp);