/*
 * pmpa_internals.h
 * Part of pmpa
 * Copyright (c) 2014 Philip Wernersbach
 *
 * Dual-Licensed under the Public Domain and the Unlicense.
 * Choose the one that you prefer.
 */

#ifndef HAVE_PMPA_INTERNALS_H

#include <stdbool.h>
#include <pmpa.h>

typedef struct {
	pmpa_memory_int size;
	bool allocated;
	char data;
} pmpa_memory_block;

#define PMPA_MEMORY_BLOCK_HEADER_SIZE ( sizeof(pmpa_memory_int) + sizeof(bool) )

#ifdef PMPA_UNIT_TEST
#define PMPA_STATIC_UNLESS_TESTING
extern PMPA_STATIC_UNLESS_TESTING __thread pmpa_memory_block *master_memory_block;
extern PMPA_STATIC_UNLESS_TESTING __thread pmpa_memory_int master_memory_block_size;
#else
#define PMPA_STATIC_UNLESS_TESTING static
#endif

#define HAVE_PMPA_INTERNALS_H
#endif