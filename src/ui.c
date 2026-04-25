/*
 * ui.c — User Interface & Branding
 * ==================================
 */

#include "ui.h"
#include "screen.h"
#include "string.h"
#include "system.h"
#include "memory.h"
#include <unistd.h>

static Theme current_theme;

void ui_init(void) {
    /* Default theme: Hacker (Cyberpunk green-on-black) */
    ui_set_theme("hacker");
}

int ui_set_theme(const char *theme_name) {
    if (str_compare(theme_name, "hacker") == 0) {
        current_theme.bg = BG_DEFAULT; /* Use default terminal bg to avoid messy boxes */
        current_theme.fg_primary = COLOR_GREEN;
        current_theme.fg_secondary = COLOR_WHITE;
        current_theme.fg_accent = COLOR_CYAN;
        current_theme.fg_error = COLOR_RED;
        current_theme.fg_success = COLOR_GREEN;
        return 1;
    } else if (str_compare(theme_name, "retro") == 0) {
        current_theme.bg = BG_DEFAULT;
        current_theme.fg_primary = COLOR_YELLOW;
        current_theme.fg_secondary = COLOR_WHITE;
        current_theme.fg_accent = COLOR_MAGENTA;
        current_theme.fg_error = COLOR_RED;
        current_theme.fg_success = COLOR_GREEN;
        return 1;
    } else if (str_compare(theme_name, "ocean") == 0) {
        current_theme.bg = BG_DEFAULT;
        current_theme.fg_primary = COLOR_CYAN;
        current_theme.fg_secondary = COLOR_BLUE;
        current_theme.fg_accent = COLOR_WHITE;
        current_theme.fg_error = COLOR_RED;
        current_theme.fg_success = COLOR_GREEN;
        return 1;
    }
    return 0;
}

void ui_apply_theme(void) {
    screen_set_color(current_theme.fg_primary, current_theme.bg);
}

static void print_animated(const char *text) {
    int i = 0;
    while (text[i] != '\0') {
        char buf[2] = {text[i], '\0'};
        screen_print(buf);
        usleep(15000); /* 15ms delay per char */
        i++;
    }
}

static void print_loading_step(const char *step) {
    screen_set_color(current_theme.fg_secondary, current_theme.bg);
    screen_print("  ");
    print_animated(step);
    
    /* Dot animation */
    for (int i = 0; i < 4; i++) {
        screen_print(".");
        usleep(100000); /* 100ms */
    }
    
    screen_set_color(current_theme.fg_success, current_theme.bg);
    screen_print_line(" [OK]");
}

void ui_boot_sequence(void) {
    screen_clear();
    ui_apply_theme();
    
    const char *logo[6] = {
        "  ██╗ █████╗ ██████╗ ██╗   ██╗██╗███████╗",
        "  ██║██╔══██╗██╔══██╗██║   ██║██║██╔════╝",
        "  ██║███████║██████╔╝██║   ██║██║███████╗",
        "████║██╔══██║██╔══██╗╚██╗ ██╔╝██║╚════██║",
        "╚██╔╝██║  ██║██║  ██║ ╚████╔╝ ██║███████║",
        " ╚═╝ ╚═╝  ╚═╝╚═╝  ╚═╝  ╚═══╝  ╚═╝╚══════╝"
    };
    
    int start_row = 2;
    for (int i = 5; i >= 0; i--) {
        int target_row = start_row + i;
        for (int y = 1; y <= target_row; y++) {
            
            /* Draw the falling line */
            screen_set_cursor(y, 1);
            screen_set_color(current_theme.fg_accent, current_theme.bg);
            screen_print(logo[i]);
            
            /* Draw a random "matrix tail" right above it */
            if (y > 1) {
                screen_set_cursor(y - 1, 1);
                screen_set_color(COLOR_GREEN, current_theme.bg);
                char tail[80];
                for (int c = 0; c < 45; c++) {
                    tail[c] = (c % 2 == 0) ? (33 + ((y+c*7) % 94)) : ' ';
                }
                tail[45] = '\0';
                screen_print(tail);
            }
            
            usleep(35000); /* 35ms frame rate */
            
            /* Clear the tail */
            if (y > 1) {
                screen_set_cursor(y - 1, 1);
                screen_print("                                                 ");
            }
            /* Clear the falling line IF it hasn't reached its target */
            if (y < target_row) {
                screen_set_cursor(y, 1);
                screen_print("                                                 ");
            }
        }
    }
    
    /* Set cursor below the logo */
    screen_set_cursor(start_row + 7, 1);
    screen_print_line("          ./ Jarvis-miniOS v2.0");
    screen_set_color(current_theme.fg_secondary, current_theme.bg);
    screen_print_line("          Build. Allocate. Execute.");
    screen_newline();
    
    print_loading_step("Loading Memory...");
    print_loading_step("Loading Drivers...");
    print_loading_step("Starting Shell...");
    screen_newline();
}

void ui_launcher_menu(void) {
    screen_set_color(current_theme.fg_accent, current_theme.bg);
    screen_print_line("  1 Enter Shell");
    screen_print_line("  2 Dashboard");
    screen_print_line("  3 Diagnostics");
    screen_print_line("  4 File System");
    ui_apply_theme();
    screen_newline();
}

void ui_draw_progress_bar(int current, int total, int width) {
    if (total <= 0) total = 1;
    if (current > total) current = total;
    if (current < 0) current = 0;
    
    int filled = (current * width) / total;
    
    screen_print("[");
    screen_set_color(current_theme.fg_accent, current_theme.bg);
    for (int i = 0; i < width; i++) {
        if (i < filled) screen_print("■");
        else {
            screen_set_color(current_theme.fg_secondary, current_theme.bg);
            screen_print("□");
        }
    }
    ui_apply_theme();
    screen_print("]");
}

void ui_dashboard(void) {
    screen_newline();
    screen_set_color(current_theme.fg_accent, current_theme.bg);
    screen_print_line("  ╔════════════════════════════════════════════════╗");
    screen_print_line("  ║                 SYSTEM DASHBOARD               ║");
    screen_print_line("  ╠════════════════════════════════════════════════╣");
    
    /* Fetch system stats */
    int mem_used = mem_get_used();
    int mem_total = VIRTUAL_RAM_SIZE;
    int mem_percent = (mem_used * 100) / mem_total;
    
    ui_apply_theme();
    screen_print("  ║  RAM Usage: ");
    ui_draw_progress_bar(mem_used, mem_total, 20);
    
    char pct[16];
    int_to_string(mem_percent, pct);
    screen_print(" ");
    screen_print(pct);
    screen_print_line("%             ║");
    
    /* We'll fill Process and FS stats from shell.c or here if we have globals */
    screen_set_color(current_theme.fg_accent, current_theme.bg);
    screen_print_line("  ╚════════════════════════════════════════════════╝");
    screen_newline();
    ui_apply_theme();
}

void ui_draw_footer(int mem_used, int mem_total, int process_count, const char *health) {
    screen_newline();
    screen_set_color(current_theme.bg, current_theme.fg_primary); /* Invert colors for footer */
    
    char pct[16], proc[16];
    int_to_string((mem_used * 100) / mem_total, pct);
    int_to_string(process_count, proc);
    
    screen_print(" RAM: ");
    screen_print(pct);
    screen_print("% | PROCS: ");
    screen_print(proc);
    screen_print(" | HEALTH: ");
    screen_print(health);
    
    /* Pad with spaces to clear line */
    for(int i=0; i<30; i++) screen_print(" ");
    
    screen_reset_color();
    ui_apply_theme();
    screen_newline();
}


