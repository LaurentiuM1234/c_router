#ifndef __TREE__
#define __TREE__

#include "rt_entry.h"


typedef struct node node_t;
typedef struct node* root_t;


/*
 * Build the search tree that contains all routing table entries
 * found in given file
 * @params: pathname -> path of file which contains routing table entries
 * @returns: search tree containing routing table entries
 */
root_t load_routes(const char *pathname);

/*
 * Wrapper for @search function
 * @params: destination -> destination ip (network order)
 *          root -> root of the search tree
 * @returns: routing table entry if a route is found, empty entry otherwise
 */
struct rt_entry find_route(uint32_t destination, root_t root);

/*
 * Safe delete a tree.
 * @params: root -> pointer to the root of the tree
 */
void destroy_tree(root_t *root);

#endif