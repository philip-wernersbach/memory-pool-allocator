/*
 * unit_test.c
 * Part of pmpa
 * Copyright (c) 2014 Philip Wernersbach
 *
 * Dual-Licensed under the Public Domain and the Unlicense.
 * Choose the one that you prefer.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <pmpa_internals.h>

#define TEST(a) (printf(">> %s... ", a))

#ifndef PMPA_DEBUG_WALK_MEMORY_POOL
#define OKAY { walk_memory_pool(NULL, true); printf("okay.\n"); } while(0)
#else
#define OKAY { walk_memory_pool(NULL, false); printf("okay.\n"); } while(0)
#endif

#define FAILED (failed("failed"))
#define FAILED_MSG(a) (failed(a))

#define MALLOC_STRING_LITERAL "malloc string works!"
#define CALLOC_STRING_LITERAL "calloc string works as well!"
#define REALLOC_STRING_ADDITIONAL_LITERAL " and so does realloc!"
#define REALLOC_STRING_LITERAL "malloc string works! and so does realloc!"
#define HELLO_WORLD_STRING_LITERAL "Hello World!"
#define HELLO_WORLD_REALLOC_ADDITIONAL_STRING_LITERAL " Again!"
#define HELLO_WORLD_REALLOC_STRING_LITERAL "Hello World! Again!"

static void failed(char *msg) {
	fflush(stdout);
	fflush(stderr);
	fprintf(stderr, "%s!\n", msg);
	exit(1);
}

static void walk_memory_pool(pmpa_memory_block *memory_block_test, bool quiet) {
	pmpa_memory_int current_memory_block_size = 0;
	
	if (!memory_block_test)
		memory_block_test = master_memory_block;
	
	if (quiet == false) {
		printf("\n\n");
		TEST("Walking memory pool");
		printf("\n");
	}
	
	for (int i = 0; (memory_block_test + sizeof(pmpa_memory_block)) < (master_memory_block + master_memory_block_size); i++) {
		if (quiet == false) {
			printf(">>> Memory Block #%d:\n", i);
			printf(">>>> Size: %llu\n", memory_block_test->size);
			printf(">>>> Allocated: %s\n", memory_block_test->allocated ? "true":"false");
		
			printf(">>>> Data: ");
			if (memory_block_test->allocated == true)
				printf("%s", &(memory_block_test->data));
			else
				printf("?");
		
			printf("\n");
		}
		
		current_memory_block_size += memory_block_test->size + PMPA_MEMORY_BLOCK_HEADER_SIZE;
		memory_block_test += memory_block_test->size + PMPA_MEMORY_BLOCK_HEADER_SIZE;
	}
	
	if (quiet == false) {
		printf(">>> Current Memory Block Size: %lld (Should Be: %lld)\n", current_memory_block_size, master_memory_block_size);
		printf("\n>> ... ");
	}
	
	if (current_memory_block_size != master_memory_block_size)
		FAILED_MSG("master_memory_block_size changed");
}

int main(int argc, char* argv[])
{	
	char *malloc_string = NULL;
	char *calloc_string = NULL;
	char *realloc_string = NULL;
	char *realloc_string_null = NULL;
	char *hello_world_string = NULL;
	
	printf("> Running unit tests.\n");
	
	TEST("pmpa_init_thread");
	if (!pmpa_init_thread((PMPA_MEMORY_BLOCK_HEADER_SIZE + 65536) * 1))
		FAILED;
	OKAY;
	
	TEST("pmpa_malloc");
	if  ( !(malloc_string = pmpa_malloc(strlen(MALLOC_STRING_LITERAL) + 1)) )
		FAILED;
	
	strcpy(malloc_string, MALLOC_STRING_LITERAL);
	OKAY;
	
	TEST("pmpa_calloc");
	if  ( !(calloc_string = pmpa_calloc(1, strlen(CALLOC_STRING_LITERAL) + 1)) )
		FAILED;
	
	for (int i = 0; i < strlen(CALLOC_STRING_LITERAL) + 1; i++)
		if (calloc_string[i] != 0)
			FAILED_MSG("calloc didn't return zeroed memory");
	
	strcpy(calloc_string, CALLOC_STRING_LITERAL);
	OKAY;
	
	TEST("pmpa_realloc [valid pointer] (1/2)");
	realloc_string = pmpa_malloc(strlen(malloc_string) + 1);
	strcpy(realloc_string, malloc_string);
	
	if  ( !(realloc_string = pmpa_realloc(realloc_string, strlen(realloc_string) + strlen(REALLOC_STRING_ADDITIONAL_LITERAL) + 1)) )
		FAILED;
	
	strcat(realloc_string, REALLOC_STRING_ADDITIONAL_LITERAL);
	OKAY;
	
	TEST("pmpa_realloc [NULL pointer] (2/2)");
	if  ( !(realloc_string_null = pmpa_realloc(NULL, strlen(REALLOC_STRING_LITERAL) + 1)) )
		FAILED;

	strcat(realloc_string_null, REALLOC_STRING_LITERAL);
	OKAY;
	
	TEST("malloc_string (1/2)");
	printf("%s... ", malloc_string);
	
	if (strcmp(malloc_string, MALLOC_STRING_LITERAL) != 0)
		FAILED;
	
	OKAY;
	
	TEST("pmpa_free");
	pmpa_free(malloc_string);
	OKAY;
	
	TEST("malloc_string (2/2)");
	printf("%s... ", malloc_string);
	
	if (strcmp(malloc_string, MALLOC_STRING_LITERAL) != 0)
		FAILED;
	
	OKAY;
	
	TEST("calloc_string");
	printf("%s... ", calloc_string);
	
	if (strcmp(calloc_string, CALLOC_STRING_LITERAL) != 0)
		FAILED;
	OKAY;
	
	TEST("realloc_string (1/2)");
	printf("%s... ", realloc_string);
	
	if (strcmp(realloc_string, REALLOC_STRING_LITERAL) != 0)
		FAILED;
	OKAY;
	
	TEST("realloc_string_null");
	printf("%s... ", realloc_string_null);
	
	if (strcmp(realloc_string_null, REALLOC_STRING_LITERAL) != 0)
		FAILED;
	OKAY;
	
	pmpa_free(realloc_string);
	
	TEST("hello_world_string [malloc] (1/2)");
	hello_world_string = pmpa_malloc(strlen(HELLO_WORLD_STRING_LITERAL) + 1);
	strcpy(hello_world_string, HELLO_WORLD_STRING_LITERAL);
	
	printf("%s... ", hello_world_string);
	
	if (strcmp(hello_world_string, HELLO_WORLD_STRING_LITERAL) != 0)
		FAILED;
	OKAY;
	
	pmpa_free(calloc_string);
	
	TEST("hello_world_string [realloc] (2/2)");
	hello_world_string = pmpa_realloc(hello_world_string, strlen(HELLO_WORLD_STRING_LITERAL) + strlen(HELLO_WORLD_REALLOC_ADDITIONAL_STRING_LITERAL) + 1);
	strcat(hello_world_string, HELLO_WORLD_REALLOC_ADDITIONAL_STRING_LITERAL);
	
	printf("%s... ", hello_world_string);
	
	if (strcmp(hello_world_string, HELLO_WORLD_REALLOC_STRING_LITERAL) != 0)
		FAILED;
	OKAY;
	
	TEST("realloc_string (2/2)");
	printf("%s... ", realloc_string);
	
	if (strcmp(realloc_string, REALLOC_STRING_LITERAL) != 0)
		FAILED;
	OKAY;
	
	pmpa_free(realloc_string);
	pmpa_free(hello_world_string);
	
	TEST("pmpa_uninit_thread");
	if (!pmpa_uninit_thread())
		FAILED;
	OKAY;
	
	printf("> Done!\n");
	return 0;
}