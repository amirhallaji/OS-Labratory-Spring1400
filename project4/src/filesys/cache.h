#ifndef FILESYS_CACHE_H
#define FILESYS_CACHE_H

#include "devices/block.h"
#include "devices/timer.h"
#include "threads/synch.h"
#include <list.h>

#define WRITE_BACK_INTERVAL 5*TIMER_FREQ
#define MAX_FILESYS_CACHE_SIZE 64

struct list filesys_cache;
uint32_t filesys_cache_size;
struct lock filesys_cache_lock;

struct cache_entry {
  uint8_t block[BLOCK_SECTOR_SIZE];
  block_sector_t sector;
  bool dirty;
  bool accessed;
  int open_cnt;
  struct list_elem elem;
};

void filesys_cache_init (void);
struct cache_entry *block_in_cache (block_sector_t sector);
struct cache_entry* filesys_cache_block_get (block_sector_t sector,
					     bool dirty);
struct cache_entry* filesys_cache_block_evict (block_sector_t sector,
					       bool dirty);
void filesys_cache_write_to_disk (bool halt);
void thread_func_write_back (void *aux);
void thread_func_read_ahead (void *aux);
void spawn_thread_read_ahead (block_sector_t sector);

#endif /* filesys/cache.h */
