#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <mpi.h>
#include <string.h>

#define WIDTH 50
#define HEIGHT 50
#define TOLERANCE 1e-6

void initialize(double **Temperature_previous, int local_height, int local_width);
void compute(double **Temperature, double **Temperature_previous, int local_height, int local_width, int *iteration, double *worst_dt);
void save_matrix_mpi(double **Temperature_previous, int local_height, int local_width, int rank, int size);
void local_dim(int rank, int size, int total_height, int *local_height, int *local_width);
void comm_mpi(int north, int south, double **Temperature_previous, int local_height);

int main(int argc, char *argv[]) {
    int rank, size;
    int local_height, local_width;
    double **Temperature, **Temperature_previous;
    double worst_dt;
    int iteration = 1;
    clock_t start_time, end_time;
    double execution_time;
    int north, south;

    MPI_Init(&argc, &argv);
    
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    local_dim(rank, size, HEIGHT, &local_height, &local_width);

    // Allocation de mémoire pour les matrices Temperature et Temperature_previous
    Temperature = (double **)malloc((local_height + 2) * sizeof(double *));
    Temperature_previous = (double **)malloc((local_height + 2) * sizeof(double *));
    for (int i = 0; i < local_height + 2; i++) {
        Temperature[i] = (double *)malloc((local_width + 2) * sizeof(double));
        Temperature_previous[i] = (double *)malloc((local_width + 2) * sizeof(double));
    }

    start_time = clock();

    initialize(Temperature_previous, local_height, local_width);

    compute(Temperature, Temperature_previous, local_height, local_width, &iteration, &worst_dt);

    end_time = clock();
    execution_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;

    save_matrix_mpi(Temperature_previous, local_height, local_width, rank, size);

    if (rank == 0) {
        printf("\nMax error at iteration %d was %f\n", iteration - 1, worst_dt);
        printf("\nTemps d'execution : %.6f secondes\n", execution_time);
    }

    // Libérer la mémoire
    for (int i = 0; i < local_height + 2; i++) {
        free(Temperature[i]);
        free(Temperature_previous[i]);
    }
    free(Temperature);
    free(Temperature_previous);

    MPI_Finalize();
    return 0;
}

void initialize(double **Temperature_previous, int local_height, int local_width) {
    int i, j;

    // Initialisation des conditions aux bords et de la condition initiale
    for (i = 0; i < local_height + 2; i++) {
        Temperature_previous[i][0] = 100.0; // Bord du bas
        Temperature_previous[i][local_width + 1] = 100.0; // Bord du haut
    }

    for (j = 0; j < local_width + 2; j++) {
        Temperature_previous[0][j] = 100.0; // Bord de gauche
        Temperature_previous[local_height + 1][j] = 100.0; // Bord de droite
    }

    // Initialisation de l'intérieur de la plaque à 0 degrés
    for (i = 1; i <= local_height; i++) {
        for (j = 1; j <= local_width; j++)
            Temperature_previous[i][j] = 0.0;
    }
}

void compute(double **Temperature, double **Temperature_previous, int local_height, int local_width, int *iteration, double *worst_dt) {
    double dt;
    int i, j;
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    int north = (rank - 1 + size) % size;
    int south = (rank + 1) % size;

    *worst_dt = TOLERANCE + 1.0;

    while (*worst_dt > TOLERANCE && *iteration < 100000) {
        // Calcul du laplacien en chaque point de maillage
        for (i = 1; i <= local_height; i++) {
            for (j = 1; j <= local_width; j++)
                Temperature[i][j] = 0.25 * (Temperature_previous[i + 1][j] + Temperature_previous[i - 1][j] +
                                            Temperature_previous[i][j + 1] + Temperature_previous[i][j - 1]);
        }

        // Communications avec les voisins
        comm_mpi( north, south, Temperature_previous, local_height);

        // Calcul du critère d'arrêt et mise à jour des températures
        double local_worst_dt = 0.0;
        for (i = 1; i <= local_height; i++) {
            for (j = 1; j <= local_width; j++) {
                dt = fabs(Temperature[i][j] - Temperature_previous[i][j]);
                Temperature_previous[i][j] = Temperature[i][j];
                if (dt > local_worst_dt) {
                    local_worst_dt = dt;
                }
            }
        }

        // Réduire local_worst_dt pour obtenir le maximum global
        MPI_Allreduce(&local_worst_dt, worst_dt, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);

        (*iteration)++;
    }
}


void save_matrix_mpi(double **Temperature_previous, int local_height, int local_width, int rank, int size) {
    MPI_File file;
    MPI_Offset offset;
    MPI_Status status;
    char filename[50] = "temperature_mpi.txt";

    MPI_File_open(MPI_COMM_WORLD, filename, MPI_MODE_CREATE | MPI_MODE_WRONLY, MPI_INFO_NULL, &file);
    
    MPI_File_set_view(file, rank * local_height * local_width * sizeof(double), MPI_DOUBLE, MPI_DOUBLE, "native", MPI_INFO_NULL);
    MPI_File_write_all(file, &(Temperature_previous[1][1]), local_height * local_width, MPI_DOUBLE, &status);

    MPI_File_close(&file);
}WIDTH
/*
#include <stdio.h>
#include <stdlib.h>

void save_matrix_mpi(double **Temperature_previous, int local_height, int local_width, int rank, int size) {
    FILE *file;
    char filename[50];
    sprintf(filename, "temperature_mpi.txt");

    // Ouvrir le fichier en mode écriture
    file = fopen(filename, "a"); // Utilisation de "a" pour ajouter à la fin du fichier

    if (file == NULL) {
        printf("Erreur lors de l'ouverture du fichier.\n");
        return;
    }

    // Écrire les données dans le fichier en utilisant fprintf
    for (int i = 1; i <= local_height; i++) {
        for (int j = 1; j <= local_width; j++) {
            fprintf(file, "%lf ", Temperature_previous[i][j]);
        }
        fprintf(file, "\n");
    }

    // Fermer le fichier
    fclose(file);
}*/




void local_dim(int rank, int size, int total_height, int *local_height, int *local_width) {
    *local_height = total_height / size;
    *local_width = WIDTH;
    if (rank < HEIGHT % size) (*local_height)++;
}

void comm_mpi( int north, int south, double **Temperature_previous, int local_height) {

    MPI_Sendrecv(&Temperature_previous[1][0], WIDTH + 2, MPI_DOUBLE, north, 0,
                 &Temperature_previous[local_height + 1][0], WIDTH + 2, MPI_DOUBLE, south, 0,
                 MPI_COMM_WORLD,  MPI_STATUS_IGNORE);
    MPI_Sendrecv(&Temperature_previous[local_height][0], WIDTH + 2, MPI_DOUBLE, south, 0,
                 &Temperature_previous[0][0], WIDTH + 2, MPI_DOUBLE, north, 0,
                 MPI_COMM_WORLD,  MPI_STATUS_IGNORE);
}
