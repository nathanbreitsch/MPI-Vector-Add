#include <mpi.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#define VECTOR_SIZE 1200

int main(int argc, char** argv){
   int size, rank;

   MPI_Init(&argc, &argv);
   MPI_Comm_size(MPI_COMM_WORLD, &size);//gets number of processes
   MPI_Comm_rank(MPI_COMM_WORLD, &rank);//gets process id

   int *a = NULL;
   int *b = NULL;

   if(rank == 0){ //let first process create vectors
      srand( time(NULL) );
      //send delegate ranges
      a = malloc(sizeof(int) * VECTOR_SIZE);
      b = malloc(sizeof(int) * VECTOR_SIZE);
      for(int i = 0; i < VECTOR_SIZE; i++){
         //a[i] = rand() % 100;
         //b[i] = rand() % 100;
         a[i] = 1;
         b[i] = 1;
      }
   }

   //printf("got here 1 and i am %d\n", rank);

   //allocate memory to store process's share of the computation
   int piece_size = VECTOR_SIZE / size;
   int* a_piece = malloc(sizeof(int) * piece_size);
   int* b_piece = malloc(sizeof(int) * piece_size);

   //distribute a
   MPI_Scatter(
      a,
      piece_size,
      MPI_INT,
      (void*) a_piece,
      piece_size,
      MPI_INT,
      0,
      MPI_COMM_WORLD
   );

   //distribute b
   MPI_Scatter(
      b,
      piece_size,
      MPI_INT,
      (void*) b_piece,
      piece_size,
      MPI_INT,
      0,
      MPI_COMM_WORLD
   );

   int dot_product = 0;
   for(int i = 0; i < piece_size; i++){
     dot_product += a_piece[i] * b_piece[i];
   }

   int* sub_products = NULL;
   if(rank == 0){
     sub_products = malloc(sizeof(int) * size);
   }

   //gather answers from each process
   MPI_Gather(
     &dot_product,
     1,
     MPI_INT,
     sub_products,
     size,
     MPI_INT,
     0,
     MPI_COMM_WORLD
   );

   //sum up all pieces in root node and print answer
   if(rank == 0){
     int sum = 0;
     for(int i = 0; i < size; i++){
       sum += sub_products[i];
     }
     printf("sum = %d\n", sum);
   }

   MPI_Finalize();
   return 0;
}
