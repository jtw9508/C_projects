#ifndef _linked_list_H_
#define _linked_list_H_

#include <stddef.h>

typedef int key_t;

typedef struct node_t {  
  key_t key;
  struct node_t *next;
  
} node_t;

typedef struct {
  node_t *head;
  
} linked_list;

linked_list *new_linked_list(void);
void delete_linked_list(linked_list *);

node_t *linked_list_insert(linked_list *, const key_t);
node_t *linked_list_find(const linked_list *, const key_t);
node_t *linked_list_min(const linked_list *);
node_t *linked_list_max(const linked_list *);
int linked_list_erase(linked_list *, node_t *);

int linked_list_to_array(const linked_list *, key_t *, const size_t);

#endif  // _linked_list_H_


