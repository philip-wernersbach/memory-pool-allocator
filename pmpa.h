/*
 * pmpa.h
 * Part of pmpa
 * Copyright (c) 2014 Philip Wernersbach
 *
 * Dual-Licensed under the Public Domain and the Unlicense.
 * Choose the one that you prefer.
 */

#ifndef HAVE_PMPA_H

#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>

#ifdef ARCH_32BIT
typedef uint32_t pmpa_memory_int;
#elif ARCH_64BIT
typedef uint64_t pmpa_memory_int;
#else
#error "ARCH_32BIT or ARCH_64BIT must be specified!"
#endif

/* 
 * Externally accessible API functions. 
 */

bool pmpa_init_thread(pmpa_memory_int master_memory_block_size);
bool pmpa_uninit_thread(void);

/*
 * Externally accessible C memory functions.
 */

void *pmpa_malloc(size_t size);
void *pmpa_calloc(size_t nelem, size_t elsize);
void *pmpa_realloc(void *ptr, size_t size);
void pmpa_free(void *ptr);

#ifdef PMPA_OVERRIDE_C_MEMORY_FUNCTIONS
#ifndef PMPA_NO_OVERRIDE_C_MEMORY_FUNCTIONS

#define malloc(a) pmpa_malloc(a)
#define calloc(a, b) pmpa_calloc(a, b)
#define realloc(a, b) pmpa_realloc(a, b)
#define free(a) pmpa_free(a)

#endif
#endif

#define HAVE_PMPA_H
#endif