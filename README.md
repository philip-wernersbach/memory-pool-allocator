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