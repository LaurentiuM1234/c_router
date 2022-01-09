#include "tree.h"
#include "../util.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define ENTRY_BUFF_SIZE 100

struct node {
  struct rt_entry _entry;
  struct node *_left;
  struct node *_right;
};


static node_t *alloc_node(struct rt_entry entry)
{
  node_t *node = calloc(1, sizeof(node_t));

  if (!node)
    return NULL;

  node->_entry = entry;

  return node;
}

static void swap_hops(node_t *a, node_t *b)
{
  struct rt_entry aux = a->_entry;
  a->_entry = b->_entry;
  b->_entry = aux;
}

static void insert(node_t *node, root_t root, int level)
{
  if (IS_EMPTY_ROUTE(root->_entry)) {
    // root is empty, swap hops and return
    swap_hops(node, root);
    free(node);
    return;
  }
  if (node->_entry._length > root->_entry._length) {
    swap_hops(node, root);
  }
  if (extract_value(node->_entry._prefix, level, level) == 0) {
    if (!root->_left)
      root->_left = node;
    else
      insert(node, root->_left, level + 1);
    return;
  } else {
    if (!root->_right)
      root->_right = node;
    else
      insert(node, root->_right, level + 1);
    return;
  }
}

struct rt_entry search(uint32_t destination, root_t root, int level)
{
  struct rt_entry entry = empty_entry();

  while (root != NULL) {
    if (extract_value(destination, 0, root->_entry._length - 1) == root->_entry._prefix)
      return root->_entry;

    if (extract_value(destination, level, level) == 0)
      root = root->_left;
    else
      root = root->_right;

    level += 1;
  }
  return entry;
}

static void free_tree(root_t root)
{
  if (!root)
    return;
  free_tree(root->_left);
  root->_left = NULL;
  free_tree(root->_right);
  root->_right = NULL;
  free(root);
}

void destroy_tree(root_t *root)
{
  free_tree(*root);
  *root = NULL;
}

root_t load_routes(const char *pathname)
{
  FILE *f_ptr = fopen(pathname, "r");

  if (!f_ptr)
    return NULL;

  root_t root = alloc_node(empty_entry());

  if (!root) {
    fclose(f_ptr);
    return NULL;
  }
  char buffer[ENTRY_BUFF_SIZE];

  while (fgets(buffer, ENTRY_BUFF_SIZE, f_ptr)) {
    // removing newline if there
    if (buffer[strlen(buffer) - 1] == '\n')
      buffer[strlen(buffer) - 1] = 0;
    // inserting node into the tree
    insert(alloc_node(to_entry(buffer)), root, 0);
    // cleaning the buffer
    memset(buffer, 0, ENTRY_BUFF_SIZE);
  }

  fclose(f_ptr);
  return root;
}

struct rt_entry find_route(uint32_t destination, root_t root)
{
  return search(destination, root, 0);
}