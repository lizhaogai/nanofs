/* Copyright (c) 2010 James Grenning and Contributed to Hunt Project
 * ==========================================
 *  Hunt Project - A Test Framework for C
 *  Copyright (c) 2007 Mike Karlesky, Mark VanderVoord, Greg Williams
 *  [Released under MIT License. Please refer to license.txt for details]
 * ========================================== */

#ifndef HUNT_FIXTURE_MALLOC_OVERRIDES_H_
#define HUNT_FIXTURE_MALLOC_OVERRIDES_H_

#include <stddef.h>

#ifdef HUNT_EXCLUDE_STDLIB_MALLOC
/* Define this macro to remove the use of stdlib.h, malloc, and free.
 * Many embedded systems do not have a heap or malloc/free by default.
 * This internal hunt_malloc() provides allocated memory deterministically from
 * the end of an array only, hunt_free() only releases from end-of-array,
 * blocks are not coalesced, and memory not freed in LIFO order is stranded. */
#ifndef HUNT_INTERNAL_HEAP_SIZE_BYTES
#define HUNT_INTERNAL_HEAP_SIZE_BYTES 256
#endif
#endif

/* These functions are used by the Hunt Fixture to allocate and release memory
 * on the heap and can be overridden with platform-specific implementations.
 * For example, when using FreeRTOS HUNT_FIXTURE_MALLOC becomes pvPortMalloc()
 * and HUNT_FIXTURE_FREE becomes vPortFree(). */
#if !defined(HUNT_FIXTURE_MALLOC) || !defined(HUNT_FIXTURE_FREE)

#include <stdlib.h>

#define HUNT_FIXTURE_MALLOC(size) malloc(size)
#define HUNT_FIXTURE_FREE(ptr)    free(ptr)
#else
extern void* HUNT_FIXTURE_MALLOC(size_t size);
extern void HUNT_FIXTURE_FREE(void* ptr);
#endif

#define malloc  hunt_malloc
#define calloc  hunt_calloc
#define realloc hunt_realloc
#define free    hunt_free

void *hunt_malloc(size_t size);

void *hunt_calloc(size_t num, size_t size);

void *hunt_realloc(void *oldMem, size_t size);

void hunt_free(void *mem);

#endif /* HUNT_FIXTURE_MALLOC_OVERRIDES_H_ */
