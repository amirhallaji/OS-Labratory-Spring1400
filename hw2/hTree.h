/* implementation of a H-tree */
typedef struct htNode *hTree;

/* create a new empty tree */
hTree htCreate(void);

/* free a tree */
void htDestroy(hTree t);

/* return nonzero if key is present in tree */
int htSearch(hTree t, int key);

/* insert a new element into a tree */
void htInsert(hTree t, int key);

/* print all keys of the tree in order */
void htPrintKeys(hTree t);