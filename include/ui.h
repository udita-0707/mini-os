/*
 * ui.h — User Interface & Branding for Jarvis-miniOS
 * ====================================================
 */

#ifndef UI_H
#define UI_H

/* Theme state */
typedef struct {
    int bg;
    int fg_primary;
    int fg_secondary;
    int fg_accent;
    int fg_error;
    int fg_success;
} Theme;

/* Initialize UI subsystem */
void ui_init(void);

/* Change current theme: "hacker", "retro", "ocean" */
int ui_set_theme(const char *theme_name);

/* Draw the interactive boot sequence with loading dots */
void ui_boot_sequence(void);

/* Draw the startup launcher menu */
void ui_launcher_menu(void);

/* Draw the system dashboard */
void ui_dashboard(void);

/* Draw the persistent footer at the bottom of the screen */
void ui_draw_footer(int mem_used, int mem_total, int process_count, const char *health);

/* Apply current theme colors */
void ui_apply_theme(void);

/* Draw a progress bar like [■■■□□□□□□] */
void ui_draw_progress_bar(int current, int total, int width);

#endif /* UI_H */
