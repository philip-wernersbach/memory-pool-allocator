pmpa - Phil's Memory Pool Allocator
=====================

C memory allocator based on pools, thread safe.

Features
---------------------
* Simple & Fast
 * Less than 200 lines of C code
* Pool-based allocation
* Automatically defragments the pool in a deterministic fashion, without the need for garbage collection
* Lockless, thread-safe design
 * Each thread gets its own memory pool
* No limit on memory pool or allocation sizes
* Implements all C memory allocation functions
* Optional drop-in mode so that you don't have to change your code
* Comprehensive benchmark & test suites
* Permissively licensed: Dual-licensed under the Public Domain and the Unlicense
 * Choose the one that you prefer

How to Use
---------------------
**Regular Mode**

1. Compile `pmpa.c` into your program.
2. Include `pmpa.h` in your program.
3. Use `pmpa_init_thread()` in each thread that will use pmpa, **before** you allocate memory with pmpa.
 * Example: If your program is going to allocate a maximum of 64kb of memory, use `pmpa_init_thread(65536)`.
 * The actual allocatable memory will be slightly less than what's specified, due to pmpa using some memory for pool accounting.
5. Use the `pmpa_*` versions of the C memory allocation functions.
 * Example: Use `pmpa_malloc()` where you would use `malloc()`, etc.
6. Use `pmpa_uninit_thread()` in each thread when the thread is done using pmpa.

**Drop-in Mode**

1. Follow all of the steps in Regular Mode until step 5.
2. Define `PMPA_OVERRIDE_C_MEMORY_FUNCTIONS` in your program
 * This define causes pmpa to override the standard C memory allocation functions with the `pmpa_*` ones.
3. Follow step 6.