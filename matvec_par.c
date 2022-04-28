#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <mpi.h> //Incluimos la librería mpi

#define DEBUG 1

#define N 1024

int main(int argc, char *argv[] ) {

    MPI_Comm comm;
    int i, j, carga;
    float matrix[N][N];
    float *part_matrix, *part_result;
    float vector[N];
    float result[N];
    struct timeval  tc_i, tc_f, tco_i, tco_f;
    int numprocs, rank;

    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &numprocs); //Obtenemos el número de procesos
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); //Obtenemos los identificadores de los procesos

    /* Initialize Matrix and Vector */
    for(i=0;i<N;i++) {
        vector[i] = i;
        for(j=0;j<N;j++) {
            matrix[i][j] = i+j;
        }
    }


    carga = N / numprocs; //Variable para guardar el numero de filas de la matriz asignada a cada proceso

    part_matrix = malloc(N* carga * sizeof(float));
    part_result = malloc(N* carga * sizeof(float));

    MPI_Scatter(matrix, carga*N, MPI_FLOAT,part_matrix, carga*N, MPI_FLOAT, 0,MPI_COMM_WORLD );

    gettimeofday(&tc_i, NULL);

    for(i=0;i<carga;i++) {
        part_result[i]=0;
        for(j=0;j<N;j++) {
            part_result[i] += part_matrix[(i*N)+j]*vector[j];
        }
    }
//bien
    MPI_Gather(part_result, carga, MPI_FLOAT, result, carga, MPI_FLOAT, 0, MPI_COMM_WORLD );

    gettimeofday(&tc_f, NULL);

    int microseconds = (tc_f.tv_usec - tc_i.tv_usec)+ 1000000 * (tc_f.tv_sec - tc_i.tv_sec);

    /*Display result */
    if (DEBUG){
    if(rank == 0){
        for(i=0;i<N;i++) {
          printf(" %f \t ",result[i]);
        }
    }
    } else {
    printf ("Time (seconds) = %lf\n", (double) microseconds/1E6);
    }

    MPI_Finalize();
    return 0;
    }
