/*
 * memory.h — Virtual Memory Manager for CodeOS
 * ===============================================
 * Simulates RAM using a single large byte array.
 * No built-in malloc/free used for core allocation logic.
 * stdlib.h's malloc is used ONCE to create the virtual RAM block.
 */

#ifndef MEMORY_H
#define MEMORY_H

/* Total virtual RAM size: 1 MB */
#define VIRTUAL_RAM_SIZE (1024 * 1024)

/* Alignment boundary for allocations (8 bytes) */
#define MEM_ALIGN 8

/*
 * Block header — prepended to each allocation within virtual RAM.
 * Tracks size, usage status, and owning process.
 */
typedef struct {
    int size;        /* Payload size in bytes (excluding header)         */
    int in_use;      /* 1 = allocated, 0 = free                          */
    int owner_pid;   /* PID of the process that owns this block (-1=sys) */
} MemBlockHeader;

/*
 * mem_init — Allocates the virtual RAM block.
 * Must be called once at boot before any mem_alloc calls.
 */
void mem_init(void);

/*
 * mem_shutdown — Frees the virtual RAM block.
 * Called during OS shutdown.
 */
void mem_shutdown(void);

/*
 * mem_alloc — Allocates `size` bytes in virtual RAM.
 * Uses first-fit linear search. Returns NULL on failure.
 */
void* mem_alloc(int size);

/*
 * mem_free — Marks the block pointed to by `ptr` as free.
 * ptr must have been returned by a previous mem_alloc call.
 */
void mem_free(void *ptr);

/*
 * mem_print_table — Prints the allocation table for debugging.
 * Shows each block: address offset, size, status, owner.
 */
void mem_print_table(void);

/*
 * mem_get_used — Returns total bytes currently allocated.
 */
int mem_get_used(void);

/*
 * mem_get_free — Returns total bytes currently free.
 */
int mem_get_free(void);

#endif /* MEMORY_H */
