# C Concurrent Sieve of Eratosthenes

## Introduction

This is my implementation of the C Concurrent Sieve of Eratosthenes lab given as a tutorial exercise for the second year Computing students at Imperial College London.

This project implements the Sieve of Eratosthenes algorithm for obtaining all prime numbers in a range. It solves this algorithm by using the Index abstract data structure, which is implemented internally using a doubly linked list.

The tutorial exercise is divided into two exercises: the first exercise is to make the Index data structure thread-safe using the mutex locks provided by the `pthreads` library. The second exercise is to use the threads provided by the `pthreads` library to solve the problem concurrently (by having a worker that iterates through the Index for each number).

## Getting Started

### Usage

##### Test the Index in a sequential context

`$ make sequential_index && ./sequential_index`

##### Test the Index in a concurrent context

`$ make concurrent_index && ./concurrent_index`

##### Run the Sieve of Eratosthenes algorithm up to MAX

`$ make sieve && ./sieve [max]`

##### Clean all executable files

`$ make clean`
