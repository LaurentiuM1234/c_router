#include "skel.h"
#include "routing/tree.h"
#include "arp/arp_entry.h"
#include "debug/fatal.h"
#include "queue.h"
#include "util.h"
#include "netinet/ether.h"


int main(int argc, char *argv[])
{
    packet m;
    int rc;
    struct in_addr ip_addr;


    init(argc - 2, argv + 2);

    // initializing tree for routing table
    root_t root = load_routes(argv[1]);

    if (!root)
        FATAL_ERROR("Unable to initialize tree.");

    // creating arp table
    arraylist_t *arp_table = create_list(sizeof(struct arp_entry));

    if (!arp_table) {
        destroy_tree(&root);
        FATAL_ERROR("Unable to create arp table.");
    }

    queue pending_packets = queue_create();

    if (!pending_packets) {
        destroy_list(&arp_table);
        destroy_tree(&root);
        FATAL_ERROR("Unable to create packet queue.");
    }

    queue pending_arps = queue_create();

    while (1) {
        rc = get_packet(&m);
        DIE(rc < 0, "get_message");
        /* Students will write code here */

        // extracting headers from packet
        struct ether_header *eth_header = (struct ether_header*)m.payload;
        struct iphdr *ip_header = (struct iphdr*)(m.payload + sizeof(struct ether_header));
        struct icmphdr *icmp_header = parse_icmp(m.payload);
        struct arp_header *arp_header = parse_arp(m.payload);


        if (arp_header) {
            if (arp_header->op == htons(ARPOP_REQUEST)) {
                // if the router receives an arp request, send back an arp reply
                send_arp_reply(arp_header, m.interface);
                continue;
            } else if (arp_header->op == htons(ARPOP_REPLY)) {
                // update routing table
                // free some of the queue entries
                struct arp_entry new_entry = create_entry(arp_header->spa, arp_header->sha);
                push(arp_table, &new_entry);
                remove_pending_arp(pending_arps, new_entry._dest);

                // creating aux queue to store packets
                queue aux_queue = queue_create();

                if (!aux_queue)
                    FATAL_ERROR("Unable to create auxiliary queue.");

                while (!queue_empty(pending_packets)) {
                    packet crt_packet = *(packet*)queue_deq(pending_packets);
                    // extracting current packet's ip header
                    struct iphdr *crt_ip_header = (struct iphdr*)(crt_packet.payload + sizeof(struct ether_header));
                    struct ether_header *crt_eth_header = (struct ether_header*)crt_packet.payload;

                    struct rt_entry aux_entry = find_route(crt_ip_header->daddr, root);

                    if (aux_entry._next_hop == new_entry._dest) {
                        // if destinations match, send packet
                        memcpy(crt_eth_header->ether_dhost, new_entry._mac, sizeof(crt_eth_header->ether_dhost));

                        send_packet(crt_packet.interface, &crt_packet);
                    } else {
                        queue_enq(aux_queue, &crt_packet);
                    }
                }
                while (!queue_empty(aux_queue)) {
                    queue_enq(pending_packets, queue_deq(aux_queue));
                }
                continue;
            }
        }

        // checking checksum
        if (ip_checksum(ip_header, sizeof(struct iphdr)) != 0)
            continue;

        if (ip_header->ttl <= 1) {
            // send ICMP_TIME_EXCEEDED and drop packet
            inet_aton(get_interface_ip(m.interface), &ip_addr);
            send_icmp_error(ip_header->saddr, ip_addr.s_addr, eth_header->ether_dhost,
                            eth_header->ether_shost, ICMP_TIME_EXCEEDED, ICMP_EXC_TTL, m.interface);

            continue;
        }


        if (icmp_header) {
            if (icmp_header->type == ICMP_ECHO && is_for_router(ip_header->daddr)) {
                // send back echo reply
                send_icmp(ip_header->saddr, ip_header->daddr, eth_header->ether_dhost,
                          eth_header->ether_shost, ICMP_ECHOREPLY, 0, m.interface, icmp_header->un.echo.id, icmp_header->un.echo.sequence);

                continue;
            }
        }

        // find route in routing table
        struct rt_entry route = find_route(ip_header->daddr, root);


        if (IS_EMPTY_ROUTE(route)) {
            // get ip for interface on which the packet came
            inet_aton(get_interface_ip(m.interface), &ip_addr);

            // no route to destination in routing table, send ICMP_DEST_UNREACH
            send_icmp_error(ip_header->saddr, ip_addr.s_addr, eth_header->ether_dhost,
                            eth_header->ether_shost, ICMP_DEST_UNREACH, ICMP_NET_UNREACH, m.interface);

        } else {
            // route exists in routing table
            // check for entry in arp table
            struct arp_entry *arp_entry = get_entry(arp_table, route._next_hop);

            // prepare packet for sending
            ip_header->ttl--;
            ip_header->check = 0;
            ip_header->check = ip_checksum(ip_header, sizeof(struct iphdr));
            m.interface = route._interface;

            if (!arp_entry) {
                if (!has_pending_arp(pending_arps, route._next_hop)) {
                    send_arp_request(route);
                    uint32_t tmp = route._next_hop;
                    queue_enq(pending_arps, &tmp);
                }

                // add packet to pending queue
                packet pending;
                pending.interface = m.interface;
                pending.len = m.len;
                memcpy(pending.payload, m.payload, sizeof(m.payload));

                queue_enq(pending_packets, &pending);
            } else {
                memcpy(eth_header->ether_dhost, arp_entry->_mac, sizeof(eth_header->ether_dhost));

                send_packet(route._interface, &m);
            }

        }
    }
}
