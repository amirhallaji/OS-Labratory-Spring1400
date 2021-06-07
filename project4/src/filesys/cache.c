#include "filesys/cache.h"
#include "filesys/filesys.h"
#include "threads/malloc.h"
#include "threads/thread.h"

void filesys_cache_init (void)
{
  list_init(&filesys_cache);
  lock_init(&filesys_cache_lock);
  filesys_cache_size = 0;
  thread_create("filesys_cache_writeback", 0, thread_func_write_back, NULL);
}

struct cache_entry* block_in_cache (block_sector_t sector)
{
  struct cache_entry *c;
  struct list_elem *e;
  for (e = list_begin(&filesys_cache); e != list_end(&filesys_cache);
       e = list_next(e))
    {
      c = list_entry(e, struct cache_entry, elem);
      if (c->sector == sector)
	{
	  return c;
	}
    }
  return NULL;
}

struct cache_entry* filesys_cache_block_get (block_sector_t sector,
					     bool dirty)
{
  lock_acquire(&filesys_cache_lock);
  struct cache_entry *c = block_in_cache(sector);
  if (c)
    {
      c->open_cnt++;
      c->dirty |= dirty;
      c->accessed = true;
      lock_release(&filesys_cache_lock);
      return c;
    }
  c = filesys_cache_block_evict(sector, dirty);
  if (!c)
    {
      PANIC("Not enough memory for buffer cache.");
    }
  lock_release(&filesys_cache_lock);
  return c;
}

struct cache_entry* filesys_cache_block_evict (block_sector_t sector,
					       bool dirty)
{
  struct cache_entry *c;
  if (filesys_cache_size < MAX_FILESYS_CACHE_SIZE)
    {
      filesys_cache_size++;
      c = malloc(sizeof(struct cache_entry));
      if (!c)
	{
	  return NULL;
	}
      c->open_cnt = 0;
      list_push_back(&filesys_cache, &c->elem);
    }
  else
    {
      bool loop = true;
      while (loop)
	{
	  struct list_elem *e;
	  for (e = list_begin(&filesys_cache); e != list_end(&filesys_cache);
	       e = list_next(e))
	    {
	      c = list_entry(e, struct cache_entry, elem);
	      if (c->open_cnt > 0)
		{
		  continue;
		}
	      if (c->accessed)
		{
		  c->accessed = false;
		}
	      else
		{
		  if (c->dirty)
		    {
		      block_write(fs_device, c->sector, &c->block);
		    }
		  loop = false;
		  break;
		}
	    }
	}
    }
  c->open_cnt++;
  c->sector = sector;
  block_read(fs_device, c->sector, &c->block);
  c->dirty = dirty;
  c->accessed = true;
  return c;
}

void filesys_cache_write_to_disk (bool halt)
{
  lock_acquire(&filesys_cache_lock);
  struct list_elem *next, *e = list_begin(&filesys_cache);
  while (e != list_end(&filesys_cache))
    {
      next = list_next(e);
      struct cache_entry *c = list_entry(e, struct cache_entry, elem);
      if (c->dirty)
	{
	  block_write (fs_device, c->sector, &c->block);
	  c->dirty = false;
	}
      if (halt)
	{
	  list_remove(&c->elem);
	  free(c);
	}
      e = next;
    }
  lock_release(&filesys_cache_lock);
}

void thread_func_write_back (void *aux UNUSED)
{
  while (true)
    {
      timer_sleep(WRITE_BACK_INTERVAL);
      filesys_cache_write_to_disk(false);
    }
}

void spawn_thread_read_ahead (block_sector_t sector)
{
  block_sector_t *arg = malloc(sizeof(block_sector_t));
  if (arg)
    {
      *arg = sector + 1;
      thread_create("filesys_cache_readahead", 0, thread_func_read_ahead,
      		    arg);
    }
}

void thread_func_read_ahead (void *aux)
{
  block_sector_t sector = * (block_sector_t *) aux;
  lock_acquire(&filesys_cache_lock);
  struct cache_entry *c = block_in_cache(sector);
  if (!c)
    {
      filesys_cache_block_evict(sector, false);
    }
  lock_release(&filesys_cache_lock);
  free(aux);
}
