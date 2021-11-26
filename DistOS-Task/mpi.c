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
#include <stdbool.h>

#define SIZE 5

#define SET_PAIR_COORDS(a,b) {\
paired_coords[0] = a;\
paired_coords[1] = b;\
}

#define MPI_RECEIVE() {\
MPI_Cart_rank(port, paired_coords, &paired_rank);\
MPI_Recv(&result, 1, MPI_INT, paired_rank, 0, port, &status);\
}

#define MPI_SEND() {\
MPI_Cart_rank(port, paired_coords, &paired_rank);\
MPI_Send(&stored_value, 1, MPI_INT, paired_rank, 0, port);\
}

#define RES_CMP() {\
if (result >= stored_value) stored_value = result;\
}

int main(int argc, char** argv) {
  
    // Initialize the MPI environment
  MPI_Init(&argc, &argv);
  
    // Get the number of processes and ranks
  int rank, tasks;
  MPI_Comm port;
  MPI_Comm_size(MPI_COMM_WORLD, &tasks);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  
  if (tasks != SIZE*SIZE)
  {
    if (rank == 0){
      printf("This application is meant to be run with %d processes.\nUse orterun -np 25 <executable file>", SIZE*SIZE);
      MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }
    MPI_Barrier(MPI_COMM_WORLD);
  }
  
  
    // size of transputer matrix: 5*5
  int size[2];
  int periodic[2];
  int coords[2];
  
  size[0] = size[1] = SIZE;
  periodic[0] = periodic[1] = 1;
  
  MPI_Cart_create(MPI_COMM_WORLD, 2, size, periodic, 1, &port);
  MPI_Barrier(port);
  MPI_Cart_coords(port, rank, 2, coords);
  
  printf("Rank: %2d – MPI Cart created of 2 dims, %2d size\n", rank, size[1] * size[0]);
    // for debug
  MPI_Barrier(port);
  srand((unsigned) (time(NULL) + coords[0] + coords[1] + rank));
  int stored_value = (rand() * coords[0] + coords[1]) % 1000 ;
  
    // storage and coordinates variables
  int result = 0;
  int paired_coords[2] = {0};
  int paired_rank = 0;
  MPI_Status status = {0};
  
  MPI_Barrier(port);
  printf("(%d,%d): Init succ, rank: %2d, val: %3d\n", coords[0], coords[1], rank, stored_value);
  
    // Step 1
  MPI_Barrier(port);
  int switcher = coords[0] * 10 + coords[1];
  
  switch(switcher){
      
    case 2:
    case 4:
        // the blue ones
      SET_PAIR_COORDS(coords[0], coords[1] - 1);
      MPI_SEND();
      break;
      
    case 1:
    case 3:
        // the red ones
      SET_PAIR_COORDS(coords[0], coords[1] + 1);
      MPI_RECEIVE();
      RES_CMP();
      break;
      
    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
    case 30:
    case 31:
    case 32:
    case 33:
    case 34:
        // the yellow ones
      SET_PAIR_COORDS(coords[0] + 1, coords[1]);
      MPI_SEND();
      break;
      
    case 20:
    case 21:
    case 22:
    case 23:
    case 24:
    case 40:
    case 41:
    case 42:
    case 43:
    case 44:
        // the green ones
      SET_PAIR_COORDS(coords[0] - 1, coords[1]);
      MPI_RECEIVE();
      RES_CMP();
      break;
      
    case 0:
        // don't touch 0 0 yet
      break;
      
    default:
        // in case we happen to somehow get here – abort with error
      MPI_Abort(port, MPI_ERR_TOPOLOGY);
      break;
  }
  
    // wait for everyone to exchange their values
  MPI_Barrier(port);
  
    // Step 2
  
  switch(switcher){
      
    case 3:
        // the blue ones
      SET_PAIR_COORDS(coords[0], coords[1] - 2);
      MPI_SEND();
      break;
      
    case 1:
        // the red ones
      SET_PAIR_COORDS(coords[0], coords[1] + 2);
      MPI_RECEIVE();
      RES_CMP();
      break;
      
    case 20:
    case 21:
    case 22:
    case 23:
    case 24:
        // the yellow ones
      SET_PAIR_COORDS(coords[0] + 2, coords[1]);
      MPI_SEND();
      break;
      
      
    case 40:
    case 41:
    case 42:
    case 43:
    case 44:
        // the green ones
      SET_PAIR_COORDS(coords[0] - 2, coords[1]);
      MPI_RECEIVE();
      RES_CMP();
      break;
      
    default:
        // all other cases are done, MPI_Topology checked at step 1
      break;
  }
  
  MPI_Barrier(port);
  
    // Step 3
  
  switch(switcher){
      
    case 42:
        // the blue ones
      SET_PAIR_COORDS(0, 1);
      MPI_SEND();
      break;
      
    case 1:
        // the red ones
      SET_PAIR_COORDS(4, 2);
      MPI_RECEIVE();
      RES_CMP();
      break;
      
    case 43:
    case 44:
        // the yellow ones
      SET_PAIR_COORDS(coords[0], coords[1] - 3);
      MPI_SEND();
      break;
      
      
    case 40:
    case 41:
        // the green ones
      SET_PAIR_COORDS(coords[0], coords[1] + 3);
      MPI_RECEIVE();
      RES_CMP();
      break;
      
    default:
        // all other cases are done, MPI_Topology checked at step 1
      break;
  }
  
  MPI_Barrier(port);
  
    // Step 4
  
  switch(switcher){
      
    case 1:
    case 41:
        // the yellow ones
      SET_PAIR_COORDS(coords[0], coords[1] - 1);
      MPI_SEND();
      break;
      
    case 0:
    case 40:
        // the green ones
      SET_PAIR_COORDS(coords[0], coords[1] + 1);
      MPI_RECEIVE();
      RES_CMP();
      break;
      
    default:
        // all other cases are done, MPI_Topology checked at step 1
      break;
  }
  
  MPI_Barrier(port);
  
    // Step 5
  
  switch(switcher){
      
    case 40:
        // the yellow ones
      SET_PAIR_COORDS(coords[0] - 4, coords[1]);
      MPI_SEND();
      break;
      
    case 0:
        // the green ones
      SET_PAIR_COORDS(coords[0] + 4, coords[1]);
      MPI_RECEIVE();
      RES_CMP();
      break;
      
    default:
        // all other cases are done, MPI_Topology checked at step 1
      break;
  }
  
  MPI_Barrier(port);
  
  if (coords[0] == 0 && coords[1] == 0){
    printf("(%d,%d): Received max value equalling to %d\n", coords[0], coords[1], stored_value);
  }
    // Finalize the MPI environment.
  MPI_Finalize();
  
}
