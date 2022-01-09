#include "util.h"
#include "arp/arp_entry.h"
#include <netinet/ether.h>


int nmask_length(uint32_t mask)
{
  int length = 0;

  while (mask) {
    length += 1;
    mask = mask >> 1U;
  }

  return length;
}

uint32_t extract_value(uint32_t initial_value, unsigned int start, unsigned int stop)
{
  return (initial_value << (31U - stop)) >> (31U - stop + start);
}

int is_for_router(uint32_t destination)
{
    for (int i = 0; i < ROUTER_NUM_INTERFACES; i++) {
        struct in_addr ip_addr;
        inet_aton(get_interface_ip(i), &ip_addr);
        if (ip_addr.s_addr == destination)
            return 1;
    }
    return 0;
}

void send_arp_reply(struct arp_header *recv_packet, int interface)
{
    struct in_addr ip_addr;
    struct ether_header eth;
    memset(&eth, 0 ,sizeof(struct ether_header));

    memcpy(eth.ether_dhost, recv_packet->sha, sizeof(eth.ether_dhost));
    get_interface_mac(interface, eth.ether_shost);

    eth.ether_type = htons(ETHERTYPE_ARP);

    ip_addr.s_addr = recv_packet->tpa;
    inet_aton(get_interface_ip(interface), &ip_addr);
    send_arp(recv_packet->spa, recv_packet->tpa, &eth, interface, htons(ARPOP_REPLY));
}

void send_arp_request(struct rt_entry rt_entry)
{
    // setting up ethernet header
    struct ether_header eth_header;
    memset(&eth_header, 0, sizeof(struct ether_header));
    hwaddr_aton("ff:ff:ff:ff:ff", eth_header.ether_dhost);
    get_interface_mac(rt_entry._interface, eth_header.ether_shost);
    eth_header.ether_type = htons(ETHERTYPE_ARP);

    // setting up ip addresses
    struct in_addr src;
    inet_aton(get_interface_ip(rt_entry._interface), &src);

    // sending arp request
    send_arp(rt_entry._next_hop, src.s_addr, &eth_header, rt_entry._interface, htons(ARPOP_REQUEST));
}

int has_pending_arp(queue pending_arps, uint32_t next_hop)
{
    queue aux_queue = queue_create();
    int result = 0;

    while (!queue_empty(pending_arps) && !result) {
        uint32_t tmp = *(uint32_t*)queue_deq(pending_arps);
        queue_enq(aux_queue, &tmp);
        if (tmp == next_hop)
           result = 1;
    }
    while (!queue_empty(aux_queue)) {
        queue_enq(pending_arps, queue_deq(aux_queue));
    }
    return result;
}

void remove_pending_arp(queue pending_arps, uint32_t next_hop)
{
    queue aux_queue = queue_create();

    while (!queue_empty(pending_arps)) {
        uint32_t tmp = *(uint32_t*)queue_deq(pending_arps);

        if (tmp != next_hop)
            queue_enq(aux_queue, &tmp);
    }
    while (!queue_empty(aux_queue)) {
        queue_enq(pending_arps, queue_deq(aux_queue));
    }
}