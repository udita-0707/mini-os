/*
 * net.c — Networking Simulation
 * ===============================
 */

#include "net.h"
#include "system.h"
#include "screen.h"
#include "string.h"
#include <unistd.h>

typedef struct {
    char ip[16];
    char hostname[32];
    int latency; /* ms */
} Host;

static Host host_table[4] = {
    {"127.0.0.1", "localhost", 1},
    {"192.168.1.1", "router", 5},
    {"8.8.8.8", "dns", 25},
    {"10.0.0.5", "db-server", 15}
};

void net_init(void) {
    /* Nothing to init */
}

static int get_host_idx(const char *ip) {
    for (int i = 0; i < 4; i++) {
        if (str_compare(host_table[i].ip, ip) == 0 || str_compare(host_table[i].hostname, ip) == 0) {
            return i;
        }
    }
    return -1;
}

int net_ping(const char *ip) {
    int idx = get_host_idx(ip);
    if (idx == -1) {
        sys_print_line("ping: unknown host");
        return -1;
    }
    
    screen_print("PING ");
    screen_print(host_table[idx].ip);
    screen_print(" (");
    screen_print(host_table[idx].hostname);
    sys_print_line("): 56 data bytes");
    
    for (int i = 1; i <= 4; i++) {
        usleep(host_table[idx].latency * 1000); /* wait simulated latency */
        screen_print("64 bytes from ");
        screen_print(host_table[idx].ip);
        screen_print(": icmp_seq=");
        sys_print_int(i);
        screen_print(" time=");
        sys_print_int(host_table[idx].latency);
        sys_print_line(" ms");
        usleep(1000000); /* 1s delay between pings */
    }
    
    return 0;
}

int net_send(const char *ip, const char *msg) {
    int idx = get_host_idx(ip);
    if (idx == -1) {
        sys_print_line("send: unknown host");
        return -1;
    }
    
    sys_print_line("Establishing connection...");
    usleep(host_table[idx].latency * 2000);
    
    sys_print_line("Sending message...");
    usleep(str_length(msg) * 10000); /* Simulated transfer time based on length */
    
    screen_print("Message delivered to ");
    screen_print(host_table[idx].hostname);
    sys_print_line(" successfully.");
    
    return 0;
}
