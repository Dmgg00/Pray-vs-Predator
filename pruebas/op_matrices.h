#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

typedef struct {
    int filas;
    int columnas;
    int8_t** datos;  
} MatrizPesos;

MatrizPesos* multiplicar_matrices(MatrizPesos* A, MatrizPesos* B);
MatrizPesos* crear_matriz_pesos(int filas, int columnas);
void liberar_matriz_pesos(MatrizPesos* m);
void imprimir_matriz_pesos(MatrizPesos* m);
void llenar_matriz_aleatoria(MatrizPesos* m);

MatrizPesos* crear_matriz_pesos(int filas, int columnas) {
    MatrizPesos* m = (MatrizPesos*)(malloc(sizeof(MatrizPesos)));
    m->filas = filas;
    m->columnas = columnas;
    m->datos = (int8_t **)(malloc(filas * sizeof(int8_t*)));
    for (int i = 0; i < filas; i++) {
        m->datos[i] = (int8_t *)(calloc(columnas, sizeof(int8_t)));  
    }

    llenar_matriz_aleatoria(m);
    return m;
}

void liberar_matriz_pesos(MatrizPesos* m) {
    for (int i = 0; i < m->filas; i++) {
        free(m->datos[i]);
    }
    free(m->datos);
    free(m);
}

void imprimir_matriz_pesos(MatrizPesos* m) {
    for (int i = 0; i < m->filas; i++) {
        for (int j = 0; j < m->columnas; j++) {
            printf("%4d ", m->datos[i][j]);
        }
        printf("\n");
    }
}

void llenar_matriz_aleatoria(MatrizPesos* m) {
    for (int i = 0; i < m->filas; i++) {
        for (int j = 0; j < m->columnas; j++) {
            int suma = 0;
            // Sumar varios valores pequeños para generar una forma de campana al rededor del 0
            for (int k = 0; k < 6; k++) {
                suma += rand() % 27 - 13; // rango 
            }

            if (suma > 127) suma = 127;
            if (suma < -128) suma = -128;

            m->datos[i][j] = (int8_t)suma;
        }
    }
}


MatrizPesos* multiplicar_matrices(MatrizPesos* A, MatrizPesos* B) {
    if (A->columnas != B->filas) {
        printf("No se pueden multiplicar: columnas de A (%d) != filas de B (%d)\n", A->columnas, B->filas);
        return NULL;
    }

    MatrizPesos* resultado = crear_matriz_pesos(A->filas, B->columnas);

    for (int i = 0; i < resultado->filas; i++) {
        for (int j = 0; j < resultado->columnas; j++) {
            float suma = 0.0f;

            for (int k = 0; k < A->columnas; k++) {
                // normalizar en la operacion
                float valA = A->datos[i][k] / 127.0f; 
                float valB = B->datos[k][j] / 127.0f; 
                suma += valA * valB;
            }


            int valFinal = (int)(suma * 127.0f);

            if (valFinal > 127) valFinal = 127;
            if (valFinal < -128) valFinal = -128;

            resultado->datos[i][j] = (int8_t)valFinal;
        }
    }

    return resultado;
}

