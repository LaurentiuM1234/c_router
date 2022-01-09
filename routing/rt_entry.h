#ifndef __RTE__
#define __RTE__

#include <stdint.h>
#define IS_EMPTY_ROUTE(entry)((entry)._length == -1)

// the routing table entry
struct rt_entry {
  uint32_t _prefix;
  uint32_t _next_hop;
  int _length;
  int _interface;
};

/*
 * Create an empty routing table entry.
 * @returns: no return
 */
struct rt_entry empty_entry(void);

/*
 * Convert given string to routing table entry.
 * @params: buffer -> string to be converted
 * @returns: converted string
 */
struct rt_entry to_entry(char *buffer);

#endif