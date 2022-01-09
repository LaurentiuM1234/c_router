#include "rt_entry.h"
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include "../util.h"

#define DELIMITER " "

struct rt_entry empty_entry(void)
{
  struct rt_entry entry = {._length = -1, ._next_hop = 0, ._interface = 0, ._prefix = 0};

  return entry;
}


// information is kept in network-order (big endian)
struct rt_entry to_entry(char *buffer)
{
  struct rt_entry entry;

  struct in_addr addr;

  // extracting prefix
  char *c_ptr = strtok(buffer, DELIMITER);
  inet_aton(c_ptr, &addr);
  entry._prefix = addr.s_addr;

  // extracting next hop
  c_ptr = strtok(NULL, DELIMITER);
  inet_aton(c_ptr, &addr);
  entry._next_hop = addr.s_addr;

  // extracting nmask length
  c_ptr = strtok(NULL, DELIMITER);
  inet_aton(c_ptr, &addr);
  entry._length = nmask_length(addr.s_addr);

  // extracting port number
  c_ptr = strtok(NULL, DELIMITER);
  entry._interface = atoi(c_ptr);

  return entry;
}