#ifndef __ARP_ENTRY__
#define __ARP_ENTRY__

#include <stdint.h>
#include "arraylist.h"
#define MAC_SIZE 6

struct arp_entry {
    uint32_t _dest;
    uint8_t _mac[MAC_SIZE];
};

/*
 * Get arp entry which has the same ip address as given destination
 * @params: arp_table -> the arp table
 *          destination -> destination ip address
 * @returns: reference to matching arp entry if found, NULL otherwise
 */
struct arp_entry *get_entry(arraylist_t *arp_table, uint32_t destination);

/*
 * Create arp entry from given parameters
 * @params: destination -> destination ip address
 *          mac -> mac address of destination
 * @returns: new arp entry
 */
struct arp_entry create_entry(uint32_t destination, uint8_t *mac);



#endif