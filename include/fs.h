/*
 * fs.h — Virtual File System
 * ============================
 */

#ifndef FS_H
#define FS_H

#define MAX_FILES 32

#define PERM_RW 0
#define PERM_RO 1

typedef struct {
    char name[32];
    char *data;      /* Pointer to allocated memory */
    int size;
    int permissions; 
    int in_use;
} File;

void fs_init(void);
int fs_touch(const char *name);
int fs_write(const char *name, const char *data);
int fs_read(const char *name);
int fs_rm(const char *name);
void fs_ls(void);
int fs_chmod(const char *name, const char *perms);

/* Returns total number of files */
int fs_get_count(void);

#endif /* FS_H */
