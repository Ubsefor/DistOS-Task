//
//  main.c
//  DistOS-Task
//
//  Created by Alexander Makhov on 04.11.2021.
//

#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#ifndef SIZE
#define SIZE 5
#endif

int main(int argc, char** argv) {
  
  // Initialize the MPI environment
  MPI_Init(&argc, &argv);
  
  // Get the number of processes and ranks
  int rank, tasks;
  MPI_Comm port;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &tasks);
  
  // size of transputer matrix: 5*5
  int size[2] = {5, 5};
  int periodic[2] = {0};
  
  MPI_Cart_create(MPI_COMM_WORLD, 2, size, periodic, 0, &port);
  int coords[2] = {0};
  
  // nanosleep for processes to init correctly (needed on my system for some reason)
  struct timespec tim, tim2;
  tim.tv_sec  = 0;
  tim.tv_nsec = 5000000000L;
  nanosleep(&tim, &tim2);
  
  MPI_Cart_coords(port, rank, 2, coords);
  srand(rank + 4);
  int a = rand() % 1000;
  printf("Coords of process #%d: (%d, %d)\n", rank, coords[0], coords[1]);
  printf("Elem: a[%d][%d] = %d\n", coords[0], coords[1], a);
  
  // Finalize the MPI environment.
  MPI_Finalize();
}
