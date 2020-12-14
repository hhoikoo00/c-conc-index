#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>

#include "Index.h"

//---------------------------------------------------------------------------\\

void *thread_1_work(void * index_ptr) {
  
  // extract worker arguments
  struct index *idx = (struct index *) index_ptr;
  bool b;
  
  // loops adding and removing 1 from the index, searching in-between
  for(int cnt = 1; cnt <= 100; cnt++){
    b = index_insert(idx, 1);
    printf("t1:insert 1 : %s\n", b ? "true" : "false");
    
    b = index_search(idx, 1);
    printf("t1:search 1 : %s\n", b ? "true" : "false");

    b = index_remove(idx, 1);
    printf("t1:remove 1 : %s\n", b ? "true" : "false");    

    b = index_search(idx, 1);
    printf("t1:search 1 : %s\n", b ? "true" : "false");    
      
    b = index_insert(idx, 1);
    printf("t1:insert 1 : %s\n", b ? "true" : "false");

    b = index_search(idx, 1);
    printf("t1:search 1 : %s\n", b ? "true" : "false");
  }
  
  printf("thread 1 finished!\n");
  // this function must return something
  return NULL;
  
}

void *thread_2_work(void * index_ptr) {

  // extract worker arguments
  struct index *idx = (struct index *)index_ptr;  
  bool b;
  
  // loops adding and removing 2 from the index with pauses to search
  for(int cnt = 1; cnt<=100; cnt++){
    b = index_insert(idx, 2);
    printf("t2:insert 2 : %s\n", b ? "true" : "false");
    
    b = index_search(idx, 2);
    printf("t2:search 2 : %s\n", b ? "true" : "false");

    b = index_remove(idx, 2);
    printf("t2:remove 2 : %s\n", b ? "true" : "false");    

    b = index_search(idx, 2);
    printf("t2:search 2 : %s\n", b ? "true" : "false");    
      
    b = index_insert(idx, 2);
    printf("t2:insert 2 : %s\n", b ? "true" : "false");

    b = index_search(idx, 2);
    printf("t2:search 2 : %s\n", b ? "true" : "false");    
  }
  
  printf("thread 2 finished!\n");
  // this function must return something
  return NULL;
  
}

void *thread_3_work(void * index_ptr) {
  
  // extract worker arguments
  struct index *idx = (struct index *)index_ptr;  
  bool b;
  
  // disruptive loop adding and removing 1 and 2 from the index
  for(int cnt = 1; cnt<=100; cnt++){
    b = index_insert(idx, 1);
    printf("t3:insert 1 : %s\n", b ? "true" : "false");    
    
    b = index_insert(idx, 2);
    printf("t3:insert 2 : %s\n", b ? "true" : "false");    
    
    b = index_remove(idx, 1);
    printf("t3:remove 1 : %s\n", b ? "true" : "false");    
    
    b = index_remove(idx, 2);
    printf("t3:remove 2 : %s\n", b ? "true" : "false");    
    
    b = index_insert(idx, 1);
    printf("t3:insert 1 : %s\n", b ? "true" : "false");
    
    b = index_insert(idx, 2);
    printf("t3:insert 2 : %s\n", b ? "true" : "false");    
  }
  
  printf("thread 3 finished!\n");
  // this function must return something
  return NULL;
  
}

int main(int argc, char ** argv) {
  
  printf("Basic Concurrent Index Test\n");

  // Idea: start up 3 threads and have them all hit the index simultaneously.

  struct index my_index;
  index_init(&my_index, NULL, 0);
  
  // check the initial state of the index
  printf("print index forwards:\n");
  index_print(&my_index, '<');
  printf("\n");

  printf("starting 3 threads that concurrently modify the index...\n");

  // initialise references to the worker threads    
  pthread_t thread_1;
  pthread_t thread_2;
  pthread_t thread_3;
    
  // create the worker threads
  pthread_create(&thread_1, NULL, thread_1_work, &my_index);
  pthread_create(&thread_2, NULL, thread_2_work, &my_index);
  pthread_create(&thread_3, NULL, thread_3_work, &my_index);
    
  // wait for the worker threads to complete
  pthread_join(thread_1, NULL);
  pthread_join(thread_2, NULL);
  pthread_join(thread_3, NULL);
  
  // check the final state of the index
  printf("print index forwards:\n");
  index_print(&my_index, '<');
  printf("\n");

  printf("print index backwards:\n");
  index_print(&my_index, '>');
  printf("\n");
  
}
