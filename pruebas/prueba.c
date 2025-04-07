#include "op_matrices.h"

int main() {
    srand(time(NULL)); // Semilla aleatoria

    int filas = 3, columnas = 3;
    MatrizPesos* pesos = crear_matriz_pesos(1, 250);
    MatrizPesos* m = crear_matriz_pesos(250, 10);

    // for (int i = 0; i < m->filas; i++) {
    //     for (int j = 0; j < m->columnas; j++) {
    //         if(i==j){
    //             m->datos[i][j] = (int8_t)1;

    //         } else {
    //             m->datos[i][j] = (int8_t)0;
    //         }
    //     }
    // }

    MatrizPesos* resultado = crear_matriz_pesos(filas, columnas);
    resultado = multiplicar_matrices(pesos,m);

    printf("Matriz de pesos aleatorios (int8_t):\n");
    imprimir_matriz_pesos(pesos);
    printf("\n");
    imprimir_matriz_pesos(m);
    printf("\n");
    imprimir_matriz_pesos(resultado);
    printf("\n");
    liberar_matriz_pesos(m);
    liberar_matriz_pesos(resultado);
    liberar_matriz_pesos(pesos);
    return 0;
}
