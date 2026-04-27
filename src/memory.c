/*
 * memory.c — Virtual Memory Manager for CodeOS
 * ===============================================
 * Acts as the physical RAM layer, tracking a simulated 1MB byte array.
 * Key Details: Uses a sequential bump-allocator or tracked block strategy. 
 * It maintains a `MemoryBlock` array to track active pointers, size, and ownership (PID). 
 *
 * Layout:
 *   [MemBlockHeader | payload ... ] [MemBlockHeader | payload ... ] ...
 *
 * stdlib.h is used ONLY here, ONLY once, to create the virtual RAM.
 */

#include <stdio.h>
#include <stdlib.h>   /* ONLY for initial malloc/free of virtual RAM */
#include "memory.h"
#include "string.h"
#include "math.h"

/* The virtual RAM — a single contiguous block of bytes */
static char *virtual_ram = NULL;

/* ── Helpers ──────────────────────────────────────────────────────── */


/* ── mem_init ─────────────────────────────────────────────────────── */
void mem_init(void) {
    /* Allocate the virtual RAM — ONLY use of stdlib malloc */
    virtual_ram = (char *)malloc(VIRTUAL_RAM_SIZE);
    if (virtual_ram == NULL) {
        printf("FATAL: Failed to allocate virtual RAM\n");
        exit(1);
    }

    /* Zero out all memory manually (no memset) */
    for (int i = 0; i < VIRTUAL_RAM_SIZE; i++) {
        virtual_ram[i] = 0;
    }

    /* Create one large free block spanning all of virtual RAM */
    MemBlockHeader *initial = (MemBlockHeader *)virtual_ram;
    initial->size = VIRTUAL_RAM_SIZE - (int)sizeof(MemBlockHeader);
    initial->in_use = 0;
    initial->owner_pid = -1;
}

/* ── mem_shutdown ─────────────────────────────────────────────────── */
void mem_shutdown(void) {
    if (virtual_ram != NULL) {
        free(virtual_ram);   /* ONLY use of stdlib free */
        virtual_ram = NULL;
    }
}

/* ── mem_alloc ────────────────────────────────────────────────────── */
/*
 * First-fit allocator:
 * 1. Walk through the block list
 * 2. Find the first free block large enough
 * 3. Split it if there's enough room for another block
 * 4. Return a pointer to the payload (after the header)
 */
void* mem_alloc(int size) {
    if (size <= 0 || virtual_ram == NULL) {
        return NULL;
    }

    /* Align requested size using math library */
    int aligned_size = math_align_up(size, MEM_ALIGN);
    int header_size = (int)sizeof(MemBlockHeader);

    int offset = 0;

    while (offset + header_size <= VIRTUAL_RAM_SIZE) {
        MemBlockHeader *block = (MemBlockHeader *)(virtual_ram + offset);

        if (!block->in_use && block->size >= aligned_size) {
            /*
             * Found a suitable free block.
             * Split if remaining space can hold another header + 8 bytes.
             */
            int remaining = block->size - aligned_size - header_size;

            if (remaining >= MEM_ALIGN) {
                /* Create a new free block after the allocated portion */
                MemBlockHeader *new_block =
                    (MemBlockHeader *)(virtual_ram + offset + header_size + aligned_size);
                new_block->size = remaining;
                new_block->in_use = 0;
                new_block->owner_pid = -1;

                block->size = aligned_size;
            }

            block->in_use = 1;
            block->owner_pid = 0;  /* Default to system process */

            /* Return pointer to payload (just after the header) */
            return (void *)(virtual_ram + offset + header_size);
        }

        /* Move to next block */
        offset += header_size + block->size;
    }

    /* No suitable block found */
    return NULL;
}

/* ── mem_free ─────────────────────────────────────────────────────── */
void mem_free(void *ptr) {
    if (ptr == NULL || virtual_ram == NULL) {
        return;
    }

    /* The header is right before the pointer */
    int header_size = (int)sizeof(MemBlockHeader);
    MemBlockHeader *block = (MemBlockHeader *)((char *)ptr - header_size);

    /* Validate: ptr must be within virtual RAM range */
    char *block_addr = (char *)block;
    if (block_addr < virtual_ram ||
        block_addr >= virtual_ram + VIRTUAL_RAM_SIZE) {
        return;  /* Invalid pointer — silently ignore */
    }

    block->in_use = 0;
    block->owner_pid = -1;

    /*
     * Coalesce with next block if it's also free.
     * This prevents fragmentation from repeated alloc/free cycles.
     */
    int offset = (int)(block_addr - virtual_ram);
    int next_offset = offset + header_size + block->size;

    if (next_offset + header_size <= VIRTUAL_RAM_SIZE) {
        MemBlockHeader *next_block =
            (MemBlockHeader *)(virtual_ram + next_offset);
        if (!next_block->in_use) {
            block->size += header_size + next_block->size;
        }
    }
}

/* ── mem_print_table ──────────────────────────────────────────────── */
void mem_print_table(void) {
    if (virtual_ram == NULL) {
        printf("  [Memory not initialized]\n");
        return;
    }

    int header_size = (int)sizeof(MemBlockHeader);
    int offset = 0;
    int block_num = 0;
    char num_buf[16];

    printf("  ┌────────┬──────────┬──────────┬─────────┐\n");
    printf("  │ Block  │ Offset   │ Size     │ Status  │\n");
    printf("  ├────────┼──────────┼──────────┼─────────┤\n");

    while (offset + header_size <= VIRTUAL_RAM_SIZE) {
        MemBlockHeader *block = (MemBlockHeader *)(virtual_ram + offset);

        /* Safety: if size is 0 or negative, stop to avoid infinite loop */
        if (block->size <= 0) {
            break;
        }

        int_to_string(block_num, num_buf);
        printf("  │ #%-5s ", num_buf);

        int_to_string(offset, num_buf);
        printf("│ %-8s ", num_buf);

        int_to_string(block->size, num_buf);
        printf("│ %-8s ", num_buf);

        printf("│ %-7s │\n", block->in_use ? "USED" : "FREE");

        offset += header_size + block->size;
        block_num++;

        /* Only show first 16 blocks to keep output manageable */
        if (block_num >= 16) {
            printf("  │  ...   │   ...    │   ...    │   ...   │\n");
            break;
        }
    }

    printf("  └────────┴──────────┴──────────┴─────────┘\n");

    /* Summary */
    int_to_string(mem_get_used(), num_buf);
    printf("  Used: %s bytes", num_buf);

    int_to_string(mem_get_free(), num_buf);
    printf("  |  Free: %s bytes\n", num_buf);
}

/* ── mem_get_used ─────────────────────────────────────────────────── */
int mem_get_used(void) {
    if (virtual_ram == NULL) return 0;

    int header_size = (int)sizeof(MemBlockHeader);
    int offset = 0;
    int used = 0;

    while (offset + header_size <= VIRTUAL_RAM_SIZE) {
        MemBlockHeader *block = (MemBlockHeader *)(virtual_ram + offset);
        if (block->size <= 0) break;
        if (block->in_use) {
            used += block->size;
        }
        offset += header_size + block->size;
    }

    return used;
}

/* ── mem_get_free ─────────────────────────────────────────────────── */
int mem_get_free(void) {
    if (virtual_ram == NULL) return 0;

    int header_size = (int)sizeof(MemBlockHeader);
    int offset = 0;
    int free_bytes = 0;

    while (offset + header_size <= VIRTUAL_RAM_SIZE) {
        MemBlockHeader *block = (MemBlockHeader *)(virtual_ram + offset);
        if (block->size <= 0) break;
        if (!block->in_use) {
            free_bytes += block->size;
        }
        offset += header_size + block->size;
    }

    return free_bytes;
}
