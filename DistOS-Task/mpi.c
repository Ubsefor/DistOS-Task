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
#define SIZE 4
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
  int size[2] = {SIZE, SIZE};
  int periodic[2] = {1, 1};
  int coords[2] = {0};

  MPI_Cart_create(MPI_COMM_WORLD, 2, size, periodic, 0, &port);
  MPI_Cart_coords(port, rank, 2, coords);
  
  printf("Rank:%d – MPI Cart created of 2 dims, %d size", rank, size[1] * size[0]);
  srand(rank + 4);
  int stored_value = rand() % 1000;
  printf("Coords of process #%d: (%d, %d)\n", rank, coords[0], coords[1]);
  printf("Elem: a[%d][%d] = %d\n", coords[0], coords[1], stored_value);
  
    // storage and coordinates variables
  int result = 0;
  int paired_coords[2] = {0};
  int paired_rank = 0;
  MPI_Status status = {0};
  
    // Exchanging the values in 6 steps:
    // Step 1:
  paired_coords[1] = coords[1];
  
  if (coords[0] == 0 || coords[0] == 3) {
    if (coords[0] == coords[0] + 1) {
      paired_coords[0] = coords[0] + 1;
      MPI_Cart_rank(port, paired_coords, &paired_rank);
      printf("#1 MPI_Cart Successfull: (%d, %d)\n", coords[0], coords[1]);
      MPI_Send(&stored_value, 1, MPI_INT, paired_rank, 0, port);
    } else {
      paired_coords[0] = coords[0] - 1;
      MPI_Cart_rank(port, paired_coords, &paired_rank);
      printf("#1 MPI_Cart Successfull: (%d, %d)\n", coords[0], coords[1]);
      MPI_Send(&stored_value, 1, MPI_INT, paired_rank, 0, port);
    }
  } else {
    if (coords[0] == 1) {
      paired_coords[0] = coords[0] - 1;
      MPI_Cart_rank(port, paired_coords, &paired_rank);
      printf("#1 MPI_Cart Successfull: (%d, %d)\n", coords[0], coords[1]);
      MPI_Recv(&result, 1, MPI_INT, paired_rank, 0, port, &status);
    } else {
      paired_coords[0] = coords[0] + 1;
      MPI_Cart_rank(port, paired_coords, &paired_rank);
      printf("#1 MPI_Cart Successfull: (%d, %d)\n", coords[0], coords[1]);
      MPI_Recv(&result, 1, MPI_INT, paired_rank, 0, port, &status);
    }
    if (result > stored_value) {
      stored_value = result;
    }
  }
  
  printf("#1: (%d, %d) finished\n", coords[0], coords[1]);
  
  MPI_Barrier(port);
  
  if (coords[0] == 0 && coords[1] == 0){
    printf("\n\n(0, 0): Step one succesfull!\n\n");
  }
  
    // Step 2:
  if (coords[0] == 2) {
    paired_coords[0] = coords[0] - 1;
    MPI_Cart_rank(port, paired_coords, &paired_rank);
    MPI_Send(&stored_value, 1, MPI_INT, paired_rank, 0, port);
  }
  if (coords[0] == 1) {
    paired_coords[0] = coords[0] + 1;
    MPI_Cart_rank(port, paired_coords, &paired_rank);
    MPI_Recv(&result, 1, MPI_INT, paired_rank, 0, port, &status);
    
    if (result > stored_value) {
      stored_value = result;
    }
  }
  
  MPI_Barrier(port);
  
  if (coords[0] == 0 && coords[1] == 0){
    printf("\n\n(0, 0): Step two succesfull!\n\n");
  }
  
    // Step 3:
  if (coords[0] == 1 && (coords[1] == 0 || coords[1] == 3)) {
    paired_coords[0] = coords[0];
    if (coords[1] == 0) {
      paired_coords[1] = coords[1] + 1;
    } else {
      paired_coords[1] = coords[1] - 1;
    }
    MPI_Cart_rank(port, paired_coords, &paired_rank);
    MPI_Send(&stored_value, 1, MPI_INT, paired_rank, 0, port);
  }
  if (coords[0] == 1 && (coords[1] == 1 || coords[1] == 2)) {
    paired_coords[0] = coords[0];
    if (coords[1] == 1) {
      paired_coords[1] = coords[1] - 1;
    } else {
      paired_coords[1] = coords[1] + 1;
    }
    MPI_Cart_rank(port, paired_coords, &paired_rank);
    MPI_Recv(&result, 1, MPI_INT, paired_rank, 0, port, &status);
    
    if (result > stored_value) {
      stored_value = result;
    }
  }
  
  MPI_Barrier(port);
  
  if (coords[0] == 0 && coords[1] == 0){
    printf("\n\n(0, 0): Step three succesfull!\n\n");
  }
  
    // Step 4:
  if (coords[0] == 1 && coords[1] == 3) {
    paired_coords[0] = coords[0];
    paired_coords[1] = 1;
    MPI_Cart_rank(port, paired_coords, &paired_rank);
    MPI_Send(&stored_value, 1, MPI_INT, paired_rank, 0, port);
  }
  if (coords[0] == 1 && coords[1] == 1) {
    paired_coords[0] = coords[0];
    paired_coords[1] = 3;
    MPI_Cart_rank(port, paired_coords, &paired_rank);
    MPI_Recv(&result, 1, MPI_INT, paired_rank, 0, port, &status);
    if (result > stored_value) {
      stored_value = result;
    }
  }
  
  MPI_Barrier(port);
  
  if (coords[0] == 0 && coords[1] == 0){
    printf("\n\n(0, 0): Step four succesfull!\n\n");
  }
  
    // Step 5:
  if (coords[0] == 1 && coords[1] == 1) {
    paired_coords[0] = 0;
    paired_coords[1] = 1;
    MPI_Cart_rank(port, paired_coords, &paired_rank);
    MPI_Send(&stored_value, 1, MPI_INT, paired_rank, 0, port);
  }
  if (coords[0] == 0 && coords[1] == 1) {
    paired_coords[0] = 1;
    paired_coords[1] = 1;
    MPI_Cart_rank(port, paired_coords, &paired_rank);
    MPI_Recv(&result, 1, MPI_INT, paired_rank, 0, port, &status);
    
    if (result > stored_value) {
      stored_value = result;
    }
  }
  
  MPI_Barrier(port);
  
  if (coords[0] == 0 && coords[1] == 0){
    printf("\n\n(0, 0): Step five succesfull!\n\n");
  }
  
    // Step 6:
  if (coords[0] == 0 && coords[1] == 1) {
    paired_coords[0] = 0;
    paired_coords[1] = 0;
    MPI_Cart_rank(port, paired_coords, &paired_rank);
    MPI_Send(&stored_value, 1, MPI_INT, paired_rank, 0, port);
  }
  if (coords[0] == 0 && coords[1] == 0) {
    paired_coords[0] = 0;
    paired_coords[1] = 1;
    MPI_Cart_rank(port, paired_coords, &paired_rank);
    MPI_Recv(&result, 1, MPI_INT, paired_rank, 0, port, &status);
    if (result > stored_value) {
      stored_value = result;
    }
  }
  
  MPI_Barrier(port);
  
  if (coords[0] == 0 && coords[1] == 0) {
    printf("\n\n(0, 0): Step six succesfull!\n\n");
    printf("MPI_Reduce'd max: %d\n", stored_value);
  }
    // Finalize the MPI environment.
  MPI_Finalize();
  
}
