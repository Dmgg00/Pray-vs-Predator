// Estructura que representa una matriz
typedef struct {
    int filas;
    int columnas;
    float** datos;
} Matriz;

// Función que crea una matriz de tamaño filas x columnas
Matriz* crear_matriz(int filas, int columnas);

// Libera la memoria de una matriz
void liberar_matriz(Matriz* m);

// Multiplica dos matrices si es posible y devuelve una nueva
Matriz* multiplicar_matrices(const Matriz* A, const Matriz* B);

// Imprime una matriz en consola (útil para debug)
void imprimir_matriz(const Matriz* m);

Matriz* crear_matriz(int filas, int columnas) {
    Matriz* m = malloc(sizeof(Matriz));
    m->filas = filas;
    m->columnas = columnas;
    m->datos = malloc(filas * sizeof(float*));
    for (int i = 0; i < filas; i++) {
        m->datos[i] = calloc(columnas, sizeof(float)); // 0 inicializado
    }
    return m;
}

void liberar_matriz(Matriz* m) {
    if (!m) return;
    for (int i = 0; i < m->filas; i++) {
        free(m->datos[i]);
    }
    free(m->datos);
    free(m);
}

Matriz* multiplicar_matrices(const Matriz* A, const Matriz* B) {
    if (A->columnas != B->filas) {
        printf("❌ No se pueden multiplicar: columnas de A ≠ filas de B\n");
        return NULL;
    }

    Matriz* resultado = crear_matriz(A->filas, B->columnas);

    for (int i = 0; i < A->filas; i++) {
        for (int j = 0; j < B->columnas; j++) {
            for (int k = 0; k < A->columnas; k++) {
                resultado->datos[i][j] += A->datos[i][k] * B->datos[k][j];
            }
        }
    }

    return resultado;
}

void imprimir_matriz(const Matriz* m) {
    for (int i = 0; i < m->filas; i++) {
        for (int j = 0; j < m->columnas; j++) {
            printf("%.2f ", m->datos[i][j]);
        }
        printf("\n");
    }
}
