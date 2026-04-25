/*
 * system.h — System Call Layer for Jarvis-miniOS
 * ================================================
 * All higher-level modules (Shell, FS, Net, etc) use these
 * wrappers instead of calling core libraries directly.
 */

#ifndef SYSTEM_H
#define SYSTEM_H

#ifndef NULL
#define NULL ((void *)0)
#endif

/* Enable or disable trace mode (keyboard -> string -> memory -> screen) */
void sys_set_trace(int enabled);
int sys_get_trace(void);

/* Wrappers for core hardware/libraries */
void sys_print(const char *text);
void sys_print_line(const char *text);
void sys_print_int(int value);
int sys_readline(char *buffer, int max_len);

void* sys_alloc(int size);
void sys_free(void *ptr);

/* Wrappers for Process Management */
int sys_create_process(const char *name, void (*task_func)(void));
int sys_kill_process(int pid);

/* Wrappers for File System */
int sys_create_file(const char *name);
int sys_delete_file(const char *name);

/* Wrappers for Networking */
int sys_ping(const char *ip);

#endif /* SYSTEM_H */
