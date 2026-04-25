/*
 * net.h — Networking Simulation
 * ===============================
 */

#ifndef NET_H
#define NET_H

void net_init(void);

/* Simulate ping to an IP address */
int net_ping(const char *ip);

/* Simulate sending a message to an IP address */
int net_send(const char *ip, const char *msg);

#endif /* NET_H */
