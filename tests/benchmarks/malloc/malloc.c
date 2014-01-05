/*
 * malloc.c
 * Part of pmpa
 * Copyright (c) 2014 Philip Wernersbach
 *
 * Dual-Licensed under the Public Domain and the Unlicense.
 * Choose the one that you prefer.
 */

#include <stdlib.h>
#include <stdio.h>
#include <pmpa_internals.h>

#define MALLOC_TIMES 1000000

int main(int argc, char* argv[])
{
	void *memory_pointers[MALLOC_TIMES];
	
#ifdef PMPA_OVERRIDE_C_MEMORY_FUNCTIONS
	if (!pmpa_init_thread((PMPA_MEMORY_BLOCK_HEADER_SIZE + 65536) * MALLOC_TIMES)) {
		fprintf(stderr, "pmpa_init_thread failed!\n");
		return 1;
	}
#endif
	
	for (int i = 0; i < MALLOC_TIMES; i++)
	{
		memory_pointers[i] = malloc(65536);
	}
	
	for (int i = 0; i < MALLOC_TIMES; i++)
	{
		free(memory_pointers[i]);
	}

#ifdef PMPA_OVERRIDE_C_MEMORY_FUNCTIONS
	pmpa_uninit_thread();
#endif
	
	return 0;
}