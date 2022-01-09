#ifndef __UTIL__
#define __UTIL__

#include <stdint.h>
#include "include/skel.h"
#include "routing/rt_entry.h"
#include "include/queue.h"
#include "arp/arraylist.h"

/*
 * Convert netmask to length (number after the /)
 * @params: mask -> netmask to be converted(network order)
 * @returns: length of the netmask
 */
int nmask_length(uint32_t mask);

/*
 * Extract the value between given start and position bit positions
 * @params: initial_value -> value from which the extraction is done
 *          start -> start position
 *          stop -> end position
 * @returns: extracted value
 */
uint32_t extract_value(uint32_t initial_value, unsigned int start, unsigned int stop);

/*
 * Check if given ip matches any of the router's ip's
 * @params: destination -> ip of the destination
 * @returns: 1 if match, 0 otherwise
 */
int is_for_router(uint32_t destination);

/*
 * Send an arp reply from router.
 * @params: recv_packet -> arp header of the received arp request
 *          interface -> router interface on which the packet will be sent
 */
void send_arp_reply(struct arp_header *recv_packet, int interface);

/*
 * Send arp request from router to find routing table's next hop's mac address
 * @params: rt_entry -> routing table entry
 * @returns: no return
 */
void send_arp_request(struct rt_entry rt_entry);

/*
 * Check if given ip address has a pending arp request
 * @params: pending_arps -> queue containing pending arps
 *          next_hop -> target ip address
 * @returns: 1 if target ip has a pending arp request, 0 otherwise
 */
int has_pending_arp(queue pending_arps, uint32_t next_hop);

/*
 * Remove a pending arp request from queue
 * @params: pending_arps -> queue containing pending arps
 *          next_hop -> ip address for which pending arp request will
 *          be removed
 */
void remove_pending_arp(queue pending_arps, uint32_t next_hop);

#endif