#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define WIDTH 50
#define HEIGHT 50
#define TOLERANCE 1e-6


void initialize(double **Temperature) ; 
void compute(double **Temperature, double **Temperature_previous, int *iteration, double *worst_dt) ; 
void save_matrix(double **Temperature_previous) ; 

int main() {

    int i, j;
    double **Temperature, **Temperature_previous;
    double worst_dt ;
    int iteration = 1 ; 
    clock_t start_time, end_time;
    double execution_time;
    
    // Question 1 : Allocation de mémoire pour les matrices Temperature et Temperature_previous
    // Remplacez XXXXXX par les tailles exactes des matrices.  
    
    Temperature = (double **)malloc((WIDTH + 2) * sizeof(double *));
    Temperature_previous = (double **)malloc((WIDTH + 2) * sizeof(double *));
    for (i = 0; i < WIDTH + 2; i++) {
        Temperature[i] = (double *)malloc((HEIGHT + 2) * sizeof(double));
        Temperature_previous[i] = (double *)malloc((HEIGHT + 2) * sizeof(double));
    }

    start_time = clock();

    initialize(Temperature_previous) ; 

    compute(Temperature, Temperature_previous, &iteration, &worst_dt) ; 

    end_time = clock();
    execution_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;

    save_matrix(Temperature_previous) ; 
    
    printf("\nMax error at iteration %d was %f\n", iteration-1, worst_dt);
    //printf("Total time was %f seconds.\n", elapsed_time.tv_sec+elapsed_time.tv_usec);
    printf("\nTemps d'execution : %.6f secondes\n", execution_time);

    // Question 1 : et ne pas oublier de désallouer les matrices !
    // A FAIRE
    for (i = 0; i < WIDTH +2; i++) {
        free(Temperature[i]);
        free(Temperature_previous[i]);
    }
    free(Temperature);
    free(Temperature_previous);

    return 0;
}

void initialize(double **Temperature_previous) {


  int i, j ; 
  // Question 2 : Initialisation des conditions aux bords et de la condition initiale
  // A FAIRE 
    for (i = 0; i < WIDTH + 2; i++) {
        Temperature_previous[i][0] = 100.0; // Bord du bas
        Temperature_previous[i][HEIGHT + 1] = 100.0; // Bord du haut
    }

    for (j = 0; j < HEIGHT + 2; j++) {
        Temperature_previous[0][j] = 100.0; // Bord de gauche
        Temperature_previous[WIDTH + 1][j] = 100.0; // Bord de droite
    }

    // Initialisation de l'intérieur de la plaque à 0 degrés
    for (i = 1; i <= WIDTH; i++) {
        for (j = 1; j <= HEIGHT; j++) 
            Temperature_previous[i][j] = 0.0;
        
    }  
}


void compute(double **Temperature, double **Temperature_previous, int *iteration, double *worst_dt) {
 
    double dt ;
    int i, j ; 
    
    *worst_dt = TOLERANCE + 1.0;
    
    while (*worst_dt > TOLERANCE && *iteration < 10000) {
        // Question 3 : Calcul de la nouvelle température à l'aide de la discrétisation du laplacien
        for (i = 1; i <= HEIGHT; i++) {
            for (j = 1; j <= WIDTH; j++) 
                Temperature[i][j] = 0.25 * (Temperature_previous[i+1][j] + Temperature_previous[i-1][j] + Temperature_previous[i][j+1] + Temperature_previous[i][j-1]);
        }

        // Question 4 : Calcul du critère d'arrêt et mise à jour des températures
        *worst_dt = 0.0;
        for (i = 1; i <= HEIGHT; i++) {
            for (j = 1; j <= WIDTH; j++) {
                dt = fabs(Temperature[i][j] - Temperature_previous[i][j]);
                Temperature_previous[i][j] = Temperature[i][j];
                if (dt > *worst_dt) {
                    *worst_dt = dt;
                }
            }
        }        
        
        (*iteration)++ ; 
        
    }

}

void save_matrix(double **Temperature_previous) {

   int i,j ; 
   FILE *file = fopen("temperature.txt", "w") ; 
   
   for (i = 0; i<=WIDTH; i++) {
       for(j = 0; j<=HEIGHT; j++) {
          fprintf(file, "%lf ", Temperature_previous[i][j]*1);    
       }
       fprintf(file,"\n") ; 
   }

   fclose(file) ;


}


