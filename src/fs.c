/*
 * fs.c — Virtual File System
 * ============================
 * A Virtual File System (VFS) that maps 
 * logical file names to physical memory addresses.
 */

#include "fs.h"
#include "system.h"
#include "string.h"
#include "screen.h"

static File file_table[MAX_FILES];

// Clears the virtual file table.
void fs_init(void) {
    for (int i = 0; i < MAX_FILES; i++) {
        file_table[i].in_use = 0;
        file_table[i].data = NULL;
    }
}

// Helper function to linearly search for a file by name. Returns index or -1 if not found.
static int get_file_idx(const char *name) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (file_table[i].in_use && str_compare(file_table[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

// Creates an empty file entry in the table. Fails if the file exists or the table is full.
int fs_touch(const char *name) {
    if (get_file_idx(name) != -1) {
        return -1; /* File already exists */
    }
    
    for (int i = 0; i < MAX_FILES; i++) {
        if (!file_table[i].in_use) {
            str_copy(file_table[i].name, name);
            file_table[i].data = NULL;
            file_table[i].size = 0;
            file_table[i].permissions = PERM_RW;
            file_table[i].in_use = 1;
            return 0;
        }
    }
    return -2; /* No space */
}

// Allocates memory via `sys_alloc` using the length of `data`. 
// Updates the file's memory pointer and copies the string in. Fails if the file is marked read-only.
int fs_write(const char *name, const char *data) {
    int idx = get_file_idx(name);
    if (idx == -1) return -1; /* Not found */
    if (file_table[idx].permissions == PERM_RO) return -2; /* Read only */
    
    int new_size = str_length(data) + 1;
    char *new_data = (char *)sys_alloc(new_size);
    if (new_data == NULL) return -3; /* Alloc failed */
    
    str_copy(new_data, data);
    
    if (file_table[idx].data != NULL) {
        sys_free(file_table[idx].data);
    }
    
    file_table[idx].data = new_data;
    file_table[idx].size = new_size;
    return 0;
}

// Looks up the file's memory pointer and prints it using `sys_print`.
// Fails if the file doesn't exist. Prints "(empty file)" if the file exists but has no data.
int fs_read(const char *name) {
    int idx = get_file_idx(name);
    if (idx == -1) return -1;
    
    if (file_table[idx].data != NULL) {
        sys_print_line(file_table[idx].data);
    } else {
        sys_print_line("(empty file)");
    }
    return 0;
}

// Calls `sys_free` on the file's memory pointer and clears the table entry.
int fs_rm(const char *name) {
    int idx = get_file_idx(name);
    if (idx == -1) return -1;
    if (file_table[idx].permissions == PERM_RO) return -2;
    
    if (file_table[idx].data != NULL) {
        sys_free(file_table[idx].data);
    }
    file_table[idx].in_use = 0;
    return 0;
}

// Prints a formatted table of all active files, their sizes, and permissions.
void fs_ls(void) {
    sys_print_line("  NAME           SIZE      PERMS");
    sys_print_line("  ──────────────────────────────");
    
    int count = 0;
    for (int i = 0; i < MAX_FILES; i++) {
        if (file_table[i].in_use) {
            count++;
            screen_print("  ");
            screen_print(file_table[i].name);
            
            for (int p = str_length(file_table[i].name); p < 15; p++) screen_print(" ");
            
            char size_str[16];
            int_to_string(file_table[i].size, size_str);
            screen_print(size_str);
            screen_print(" B");
            
            for (int p = str_length(size_str) + 2; p < 10; p++) screen_print(" ");
            
            if (file_table[i].permissions == PERM_RW) {
                sys_print_line("rw-");
            } else {
                sys_print_line("r--");
            }
        }
    }
    
    if (count == 0) {
        sys_print_line("  (directory empty)");
    }
}

// Modifies the file's read-write or read-only boolean flag.
int fs_chmod(const char *name, const char *perms) {
    int idx = get_file_idx(name);
    if (idx == -1) return -1;
    
    if (str_compare(perms, "read-only") == 0) {
        file_table[idx].permissions = PERM_RO;
        return 0;
    } else if (str_compare(perms, "read-write") == 0) {
        file_table[idx].permissions = PERM_RW;
        return 0;
    }
    return -2; /* Invalid perm string */
}

// Returns the number of files currently in use.
int fs_get_count(void) {
    int count = 0;
    for (int i = 0; i < MAX_FILES; i++) {
        if (file_table[i].in_use) count++;
    }
    return count;
}
