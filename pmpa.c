/*
 * pmpa.c
 * Part of pmpa
 * Copyright (c) 2014 Philip Wernersbach
 *
 * Dual-Licensed under the Public Domain and the Unlicense.
 * Choose the one that you prefer.
 */

#include <string.h>

#define PMPA_NO_OVERRIDE_C_MEMORY_FUNCTIONS
#include <pmpa_internals.h>

PMPA_STATIC_UNLESS_TESTING __thread pmpa_memory_block *master_memory_block = NULL;
PMPA_STATIC_UNLESS_TESTING __thread pmpa_memory_int master_memory_block_size = 0;

/*
 * Internal functions.
 */

static void concat_sequential_blocks(pmpa_memory_block *memory_block, bool is_allocated)
{
	pmpa_memory_block *current_memory_block = memory_block;
	pmpa_memory_block *next_memory_block = NULL;
	
	if (current_memory_block->allocated != is_allocated)
		return;
	
	while ( (next_memory_block = current_memory_block + current_memory_block->size + PMPA_MEMORY_BLOCK_HEADER_SIZE) && 
			PMPA_POINTER_IS_IN_POOL(next_memory_block + PMPA_MEMORY_BLOCK_HEADER_SIZE) && 
			(next_memory_block->allocated == is_allocated) )
				current_memory_block->size += next_memory_block->size + PMPA_MEMORY_BLOCK_HEADER_SIZE;
	
	
}

static pmpa_memory_block *find_first_block(bool is_allocated, pmpa_memory_int min_size)
{
	pmpa_memory_block *memory_block = master_memory_block;
	
	while (PMPA_POINTER_IS_IN_POOL(memory_block + sizeof(pmpa_memory_block))) {
		/* If we're trying to find an block, then defragment the pool as we go along.
		 * This incurs a minor speed penalty, but not having to spend time
		 * iterating over a fragmented pool makes up for it. */
		if (is_allocated == false)
			concat_sequential_blocks(memory_block, is_allocated);
		
		if ( (memory_block->allocated == is_allocated) && (memory_block->size >= min_size) ) {
			return memory_block;
		} else {
			memory_block += memory_block->size + PMPA_MEMORY_BLOCK_HEADER_SIZE;
		}
	}
	
	return NULL;
}

static void split_block(pmpa_memory_block *memory_block, pmpa_memory_int size)
{
	pmpa_memory_block *second_memory_block = memory_block + size + PMPA_MEMORY_BLOCK_HEADER_SIZE;
	pmpa_memory_block *original_second_memory_block = memory_block + memory_block->size + PMPA_MEMORY_BLOCK_HEADER_SIZE;
	pmpa_memory_int original_memory_block_size = memory_block->size;
	
	memory_block->allocated = false;
	
	/* We can't split this block if there's not enough room to create another one. */
	if ( PMPA_POINTER_IS_IN_RANGE((second_memory_block + PMPA_MEMORY_BLOCK_HEADER_SIZE), 0, original_second_memory_block) &&
	   ( PMPA_POINTER_IS_IN_POOL(second_memory_block + sizeof(pmpa_memory_block)) ) ) {
		memory_block->size = size;
		
		second_memory_block->size = original_memory_block_size - (size + PMPA_MEMORY_BLOCK_HEADER_SIZE);
		second_memory_block->allocated = false;
	}
}

/*
 * Externally accessible API functions. 
 */

bool pmpa_init_thread(pmpa_memory_int size)
{	
	if ( (master_memory_block = malloc(size)) ) {
		master_memory_block->size = size - PMPA_MEMORY_BLOCK_HEADER_SIZE;
		master_memory_block->allocated = false;
		
		master_memory_block_size = size;
		
		return true;
	} else {
		return false;
	}
}

bool pmpa_uninit_thread(void)
{
	master_memory_block_size = 0;
	free(master_memory_block);
	
	return true;
}

/*
 * Externally accessible C memory functions.
 */

void *pmpa_malloc(size_t size)
{
	pmpa_memory_block *memory_block = find_first_block(false, size);
	
	if (memory_block) {
		split_block(memory_block, size);
		memory_block->allocated = true;
		
		return &(memory_block->data);
	} else {
		return NULL;
	}
}

void *pmpa_calloc(size_t nelem, size_t elsize)
{
	pmpa_memory_int ptr_size = nelem * elsize;
	void *ptr = pmpa_malloc(ptr_size);
	
	if (ptr) {
		memset(ptr, 0, ptr_size);
		
		return ptr;
	} else {
		return NULL;
	}
}

void *pmpa_realloc(void *ptr, size_t size)
{
	pmpa_memory_block *memory_block = NULL;
	pmpa_memory_block *new_memory_block = NULL;
	
	pmpa_memory_int memory_block_original_size = 0;
	
	/* If ptr is NULL, realloc() behaves like malloc(). */
	if (!ptr)
		return pmpa_malloc(size);
	
	memory_block = ptr - PMPA_MEMORY_BLOCK_HEADER_SIZE; 
	memory_block_original_size = memory_block->size;
	
	/* Try to cheat by concatenating the current block with contiguous 
	 * empty blocks after it, and seeing if the new block is big enough. */
	memory_block->allocated = false;
	concat_sequential_blocks(memory_block, memory_block->allocated);
	memory_block->allocated = true;
	
	if (memory_block->size >= size) {
		/* The new block is big enough, split it and use it. */
		split_block(memory_block, size);
		memory_block->allocated = true;
		
		return &(memory_block->data);
	} else {
		/* The new block is not big enough. */ 
		
		/* Restore the memory block's original size. */
		split_block(memory_block, memory_block_original_size);
		memory_block->allocated = true;
		
		/* Find another block and try to use that. */
		if ( !(new_memory_block = find_first_block(false, size)) )
			return NULL;
			
		split_block(new_memory_block, size);
		new_memory_block->allocated = true;
		
		memcpy(&(new_memory_block->data), &(memory_block->data), memory_block->size);
		
		pmpa_free(&(memory_block->data));
		return &(new_memory_block->data);
	}
	
	return NULL;
}

void pmpa_free(void *ptr)
{
	pmpa_memory_block *memory_block = ptr - PMPA_MEMORY_BLOCK_HEADER_SIZE;
	
	if (ptr == NULL)
		return;
	
	memory_block->allocated = false;
}