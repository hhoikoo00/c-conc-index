#ifndef INDEX_H
#define INDEX_H

  #include <stdbool.h>
  #include <pthread.h>

  // The node struct implements a Doubly-Linked-List element
  struct node {
    // index key of the node  
    int item;

    // pointers to previous/next nodes in the list
    struct node *prev;
    struct node *next;

    // lock for each node
    pthread_mutex_t lock;
  };

  // The index struct provides a wrapper for a Concurrent Doubely-Linked-List 
  // implementation
  struct index {
    // pointers to the head and tail of the doubly linked list
    struct node *head;
    struct node *tail;

    pthread_mutex_t lock_head;
    pthread_mutex_t lock_tail;
  };

  // initialise index IDX from the provided array SEEDS of size LENGTH
  void index_init(struct index *idx, int seeds[], int length);

  // look for element with KEY in the index IDX
  bool index_search(struct index *idx, int key);

  // add element with KEY (in order) to the index IDX
  bool index_insert(struct index *idx, int key);

  // remove element with KEY from the index IDX
  bool index_remove(struct index *idx, int key);

  // output the contents of the index IDX (pretty-printed) in specified ORDER
  void index_print(struct index *idx, char order);

  // free all elements of the index
  void index_free_elems(struct index *idx);

#endif
