#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>

#include "Index.h"

//---------------------------------------------------------------------------\\

struct worker {
  pthread_t thread;
  struct index *sieve_data;
  int num;
  int max;
};

void *sieve_task(void *worker_) {
  struct worker *worker = (struct worker *)worker_;

  struct index *sieve_data = worker->sieve_data;
  int max = worker->max;
  int num = worker->num;

  for (int i = num + num; i <= max; i += num) {
    index_remove(sieve_data, i);
  }

  // The function needs to return something
  return NULL;
}

int main(int argc, char **argv) {
  printf("Sieve of Eratosthenes Example\n");

  // Main idea: Sieve of Eratosthenes for first n primes (where n is user input)
  if (argv[1] == NULL) {
    printf("  ERROR: no input max provided!\n");
    return -1;
  }  
  int max = atoi(argv[1]);

  printf("  finding primes from 2 .. %i\n", max);

  // Create seed data for the index
  int num_index = (max + 1) - 2;
  int *index_seeds = malloc(sizeof(int) * num_index);
  for (int i = 2; i <= max; i++) {
    index_seeds[i - 2] = i;
  }

  // Create an index containing all numbers from 2 .. max
  struct index sieve_data;
  index_init(&sieve_data, index_seeds, num_index);

  free(index_seeds);

  // Create all workers and wait for them
  struct worker workers[num_index];
  struct worker *cur_worker;
  for (int i = 2; i <= max; i++) {
    cur_worker = &workers[i - 2];
    cur_worker->num = i;
    cur_worker->max = max;
    cur_worker->sieve_data = &sieve_data;
    pthread_create(&cur_worker->thread, NULL, sieve_task, cur_worker);
  }

  for (int i = 2; i <= max; i++) {
    cur_worker = &workers[i - 2];
    pthread_join(cur_worker->thread, NULL);
  }

  printf("\n");

  // When all threads have terminated, print out the final state of the index.
  // All of the values that are left in the index must be prime numbers.
  printf("print index forwards:\n");
  index_print(&sieve_data, '<');
  printf("\n");

  printf("print index backwards:\n");
  index_print(&sieve_data, '>');
  printf("\n");

  index_free_elems(&sieve_data);
}
