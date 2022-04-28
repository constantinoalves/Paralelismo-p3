#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <mpi.h> //Incluimos la librería mpi

#define DEBUG 0

#define N 2048

int main(int argc, char *argv[] ) {

    int i, j, carga;
    float matrix[N][N];
    float *part_matrix, *part_result;
    float vector[N];
    float result[N];
    struct timeval  tv1, tv2, tv3, tv4;
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

    gettimeofday(&tv1, NULL);


    MPI_Scatter(matrix, carga*N, MPI_FLOAT,part_matrix, carga*N, MPI_FLOAT, 0,MPI_COMM_WORLD );

    gettimeofday(&tv2, NULL);

    for(i=0;i<carga;i++) {
        part_result[i]=0;
        for(j=0;j<N;j++) {
            part_result[i] += part_matrix[(i*N)+j]*vector[j];
        }
    }

    gettimeofday(&tv3, NULL);

    MPI_Gather(part_result, carga, MPI_FLOAT, result, carga, MPI_FLOAT, 0, MPI_COMM_WORLD );

    gettimeofday(&tv4, NULL);

    //computation time
    int comp_time = (tv3.tv_usec - tv2.tv_usec)+ 1000000 * (tv3.tv_sec - tv2.tv_sec);

    //Communication time
    int comm_time = ((tv2.tv_usec - tv1.tv_usec)+ 1000000 * (tv2.tv_sec - tv1.tv_sec)) + ((tv4.tv_usec - tv3.tv_usec)+ 1000000 * (tv4.tv_sec - tv3.tv_sec));

    /*Display result */
    if (DEBUG){
    if(rank == 0){
        for(i=0;i<N;i++) {
          printf(" %f \t ",result[i]);
        }
    }
    } else {
        if(rank == 0){
            printf ("Process %d, Communications time (seconds)  = %lf\n", rank, (double) comm_time/1E6);
            printf ("Process %d, Data processing time (seconds) = %lf\n", rank, (double) comp_time/1E6);
            for(int n=1; n<numprocs; n++){
                MPI_Recv(&comm_time, 1, MPI_INT, n, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                printf ("Process %d, Communications time (seconds)  = %lf\n", n, (double) comm_time/1E6);
                MPI_Recv(&comp_time, 1, MPI_INT, n, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                printf ("Process %d, Data processing time (seconds) = %lf\n", n, (double) comp_time/1E6);
            }
        }else{
            MPI_Send(&comm_time, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
            MPI_Send(&comp_time, 1, MPI_INT, 0, 2, MPI_COMM_WORLD);
        }
    }
    free(part_matrix);
    free(part_result);
    MPI_Finalize();
    return 0;
}
