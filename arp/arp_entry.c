#include "arp_entry.h"
#include <string.h>

struct arp_entry *get_entry(arraylist_t *arp_table, uint32_t destination)
{
    for (int i = 0; i < size(arp_table); i++) {
        struct arp_entry *crt_entry = (struct arp_entry*)get(arp_table, i);
        if ((*crt_entry)._dest == destination)
            return crt_entry;
    }
    return NULL;
}

struct arp_entry create_entry(uint32_t destination, uint8_t *mac)
{
    struct arp_entry entry = {._dest = destination};
    memcpy(entry._mac, mac, 6);
    return entry;
}