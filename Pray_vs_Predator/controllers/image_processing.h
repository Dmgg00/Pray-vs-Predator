
#include <stdint.h>

void procesar_imagen_gris_suavizada(const uint8_t *imagen_entrada, uint8_t *imagen_salida);
void procesar_imagen_azul(const unsigned char *image, uint8_t *imagen_azul_reducida, int width, int filas_a_extraer);

void procesar_imagen_azul(const unsigned char *image, uint8_t *imagen_azul_reducida, int width, int filas_a_extraer) {
    int columnas = 10;
    int paso = width / columnas;

    for (int i = 0; i < columnas; i++) {
        int suma = 0;
        int cuenta = 0;
        int inicio = i * paso;
        int fin = (i == columnas - 1) ? width : inicio + paso;

        for (int y = 0; y < filas_a_extraer; y++) {
            for (int x = inicio; x < fin; x++) {
                int index = 4 * (y * width + x);
                suma += image[index];
                cuenta++;
            }
        }

        imagen_azul_reducida[i] = (uint8_t)(suma / cuenta);
    }
}

void procesar_imagen_gris_suavizada(const uint8_t *imagen_entrada, uint8_t *imagen_salida) {
    const int width = 752;
    const int height = 20;
    const int kernel_size = 5;
    const int half_k = kernel_size / 2;

    // Kernel simétrico para suavizado horizontal
    float kernel[kernel_size] = {0.1f, 0.2f, 0.4f, 0.2f, 0.1f};

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            float suma = 0.0f;
            float peso_total = 0.0f;

            for (int k = -half_k; k <= half_k; k++) {
                int idx = x + k;
                if (idx >= 0 && idx < width) {
                    float peso = kernel[k + half_k];
                    suma += peso * imagen_entrada[y * width + idx];
                    peso_total += peso;
                }
            }

            imagen_salida[y * width + x] = (uint8_t)(suma / peso_total);
        }
    }
}
