#include "Index.h"

#include <stdio.h>
#include <malloc.h>
#include <limits.h>
#include <sched.h>

//---------------------------------------------------------------------------\\

// Index implementation

void index_init(struct index *idx, int seeds[], int length) {
  idx->head = NULL;
  idx->tail = NULL;
  pthread_mutex_init(&idx->lock_head, NULL);
  pthread_mutex_init(&idx->lock_tail, NULL);

  // insert the seed elements into the index
  for (int i = 0; i < length; i++) {
    index_insert(idx, seeds[i]);
  }
}

bool index_search(struct index *idx, int key) {
  // set-up for index traversal
  pthread_mutex_lock(&idx->lock_head);
  struct node *cur = NULL;
  struct node *nxt = idx->head;

  int item = INT_MIN;
  
  // traverse the index elements
  while (nxt != NULL && item < key) {
    pthread_mutex_lock(&nxt->lock);
    if(nxt == idx->head) {
      pthread_mutex_unlock(&idx->lock_head);
    } else {
      pthread_mutex_unlock(&cur->lock);
    }

    cur = nxt;
    item = cur->item;

    if (item == key) {
      // found it!
      pthread_mutex_unlock(&cur->lock);
      return true;
    }

    nxt = cur->next;
  }

  if (nxt == idx->head) {
    pthread_mutex_unlock(&idx->lock_head);
  } else {
    pthread_mutex_unlock(&cur->lock);
  }

  // item not in the index so, did not find it
  return false;
}

bool index_insert(struct index *idx, int key) {  
  // create basic Node
  struct node *new_node = malloc(sizeof(struct node));
  if (new_node == NULL) {
    printf("ERROR: out of memory!\n");
    return false;
  }
  // init basic Node
  new_node->prev = NULL;
  new_node->next = NULL;
  new_node->item = key;
  pthread_mutex_init(&new_node->lock, NULL);

  pthread_mutex_lock(&idx->lock_head);
  // special case for empty index
  pthread_mutex_lock(&idx->lock_tail);
  if (idx->head == NULL && idx->tail == NULL) {
    idx->head = new_node;
    idx->tail = new_node;
    pthread_mutex_unlock(&idx->lock_tail);
    pthread_mutex_unlock(&idx->lock_head);
    return true;
  }
  pthread_mutex_unlock(&idx->lock_tail);

  // set-up for index traversal
  struct node *prv = NULL;
  struct node *cur = NULL;
  struct node *nxt = idx->head;

  // traverse the index elements
  while (nxt != NULL) {
    pthread_mutex_lock(&nxt->lock);
    if (cur == idx->head) {
      pthread_mutex_unlock(&idx->lock_head);
    } else if (cur != NULL) {
      pthread_mutex_unlock(&prv->lock);
    }

    prv = cur;
    cur = nxt;
    nxt = cur->next;

    int item = cur->item;

    if (item == key) {
      // we do not duplicate items in the index
      pthread_mutex_unlock(&cur->lock);
      if (prv != NULL) {
        pthread_mutex_unlock(&prv->lock);
      } else {
        pthread_mutex_unlock(&idx->lock_head);
      }
      free(new_node);
      return false;
    }

    if (item > key) {
      // found where the new item should go

      // update new Node pointers (dangles off of list for now)
      new_node->prev = prv;
      new_node->next = cur;

      // update back pointer of current element
      cur->prev = new_node;

      // Note: at this point the list's pointer structure is inconsistent!
      sched_yield(); // demonic scheduling here will really mess us up!

      // update forward pointer of prev element (or list head)
      if (prv != NULL) {
        prv->next = new_node;
        pthread_mutex_unlock(&prv->lock);
      } else {
        idx->head = new_node;
        pthread_mutex_unlock(&idx->lock_head);
      }
      pthread_mutex_unlock(&cur->lock);

      return true;
    }

    // goto next element
  }

  // if not inserted in loop then item belongs on the end of the list
  pthread_mutex_lock(&idx->lock_tail);

  idx->tail->next = new_node;
  new_node->prev = idx->tail;
  idx->tail = new_node;

  if (prv != NULL) {
    pthread_mutex_unlock(&prv->lock);
  } else {
    pthread_mutex_unlock(&idx->lock_head);
  }
  pthread_mutex_unlock(&idx->lock_tail);
  pthread_mutex_unlock(&cur->lock);
  
  return true;
}

bool index_remove(struct index *idx, int key) {
  // set-up for index traversal
  pthread_mutex_lock(&idx->lock_head);
  struct node *prv = NULL;
  struct node *cur = NULL;
  struct node *nxt = idx->head;

  // traverse the index elements
  while (nxt != NULL) {
    pthread_mutex_lock(&nxt->lock);
    if (cur == idx->head) {
      pthread_mutex_unlock(&idx->lock_head);
    } else if (cur != NULL) {
      pthread_mutex_unlock(&prv->lock);
    }

    prv = cur;
    cur = nxt;
    nxt = cur->next;

    int item = cur->item;

    // check each item
    if(item == key){
      // found the item to be removed

      if (nxt != NULL) {
        pthread_mutex_lock(&nxt->lock);
      } else {
        pthread_mutex_lock(&idx->lock_tail);
      }

      // update back pointer of next element (or list tail)
      
      if (nxt != NULL) {
        nxt->prev = prv;
      } else {
        idx->tail = prv;
      }

      // Note: at this point the list's pointer structure is inconsistent!
      sched_yield(); // demonic scheduling here will really mess us up!

      // update forward pointer of prev element (or list head)
      if (prv != NULL) {
        prv->next = nxt;
      } else {
        idx->head = nxt;
      }

      if (prv != NULL) {
        pthread_mutex_unlock(&prv->lock);
      } else {
        pthread_mutex_unlock(&idx->lock_head);
      }

      if (nxt != NULL) {
        pthread_mutex_unlock(&nxt->lock);
      } else {
        pthread_mutex_unlock(&idx->lock_tail);
      }

      pthread_mutex_unlock(&cur->lock);

      // free up memory for Node object
      free(cur);
      return true;
    }
  }
    
  // item not in the index, so did not remove it
  if (prv != NULL) {
    pthread_mutex_unlock(&prv->lock);
  } else {
    pthread_mutex_unlock(&idx->lock_head);
  }

  if (cur != NULL) {
    pthread_mutex_unlock(&cur->lock);
  }

  return false;
}

void index_print(struct index *idx, char order) {
  struct node *start;

  // coarse-grained locking for printing (as it can go in either way)
  pthread_mutex_lock(&idx->lock_head);
  pthread_mutex_lock(&idx->lock_tail);

  // configure traversal order
  if (order == '<') {
    start = idx->head;
  } else if (order == '>') {
    start = idx->tail;
  } else {
    printf("ERROR: unrecognised input order %c\n", order);
    return;
  }

  // set-up for index traversal
  struct node *cur = NULL; 
  struct node *nxt = start;
  printf("|--");

  // traverse the index elements
  while (nxt != NULL) {

    // show internal links
    if (nxt != start) {
      printf("<-->");
    } 

    // update next item in the list (depending on traversal order)
    cur = nxt;

    if (order == '<') {
      nxt = cur->next;
    } else {
      nxt = cur->prev;
    }
    
    // print the current item
    printf("|·|%i|·|", cur->item);
  }

  // finish the index traversal
  printf("--|\n");

  pthread_mutex_unlock(&idx->lock_head);
  pthread_mutex_unlock(&idx->lock_tail);
}

void index_free_elems(struct index *idx) {
  // set-up for index traversal
  pthread_mutex_lock(&idx->lock_head);
  struct node *cur = NULL;
  struct node *nxt = idx->head;

  // traverse the index elements
  while (nxt != NULL) {
    pthread_mutex_lock(&nxt->lock);
    if(nxt == idx->head) {
      pthread_mutex_unlock(&idx->lock_head);
    }

    cur = nxt;
    nxt = cur->next;

    pthread_mutex_unlock(&cur->lock);
    free(cur);
  }

  if (nxt == idx->head) {
    pthread_mutex_unlock(&idx->lock_head);
  }
}