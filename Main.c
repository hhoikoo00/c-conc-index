#include <stdio.h>
#include <stdbool.h>

#include "Index.h"

//---------------------------------------------------------------------------\\

int main(int argc, char ** argv) {

  printf("Basic Index Tests\n");

  // Idea: create a simple index, 
  //       search for some things, 
  //       remove some things, 
  //       then search again.  
  
  printf("Testing the node struct basics\n");
  struct node my_node;
  my_node.item = 15;
  printf("my_node's item is: %i\n\n", my_node.item);

  printf("Testing the index basics\n\n");
  int seed_data[] = {2,4,1,3,9,11,2,2,2,20};
  struct index my_index;
  index_init(&my_index, seed_data, 10);

  printf("print index forwards:\n");
  index_print(&my_index, '<');
  printf("\n");

  printf("print index backwards:\n");
  index_print(&my_index, '>');
  printf("\n");

  for(int i = 0; i<10; i++){
    bool b = index_search(&my_index, i);
    printf("search %i: %s\n", i, b ? "true" : "false");
  }

  printf("----------\n");

  for(int i = 0; i<10; i++){
    bool b = index_remove(&my_index, i);
    printf("remove %i: %s\n", i, b ? "true" : "false");
  }

  printf("----------\n");
  
  for(int i = 0; i<10; i++){
    bool b = index_search(&my_index, i);
    printf("search %i: %s\n", i, b ? "true" : "false");
  }
  
  printf("\n");

  printf("print index forwards:\n");
  index_print(&my_index, '<');
  printf("\n");

  printf("print index backwards:\n");
  index_print(&my_index, '>');
  printf("\n");  
  
}
