#include <webots/robot.h>
#include <webots/camera.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define TIME_STEP 64  // Tiempo de simulación en ms

// Incluir la biblioteca stb_image_write para guardar imágenes en formato PNG
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

// Función para generar un nombre de archivo único basado en el tiempo
char* generate_filename() {
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    
    char *filename = (char *)malloc(100 * sizeof(char));
    strftime(filename, 100, "/home/mapache/Pictures/pr_py_test/gray_image_%Y%m%d_%H%M%S.png", tm_info);
    
    return filename;
}

// Función para convertir la imagen RGB a escala de grises
void convert_to_grayscale(const unsigned char *image, unsigned char *gray_image, int width, int height) {
    int i, j;
    for (i = 0; i < height; i++) {
        for (j = 0; j < width; j++) {
            // Obtener los valores de los canales R, G y B
            int index = (i * width + j) * 4;  // Usamos *4 si la imagen está usando 4 bytes por píxel
            unsigned char r = image[index];       // Canal rojo
            unsigned char g = image[index + 1];   // Canal verde
            unsigned char b = image[index + 2];   // Canal azul

            // Calcular el valor en escala de grises utilizando la fórmula
            unsigned char gray = (unsigned char)(0.2989 * r + 0.5870 * g + 0.1140 * b);

            // Asignar el valor al canal de gris
            gray_image[i * width + j] = gray;
        }
    }
}

int main() {
    wb_robot_init(); // Inicializar Webots

    // Intentar obtener la cámara con el nombre "camera"
    WbDeviceTag camera = wb_robot_get_device("camera");

    if (camera == 0) {
        printf("⚠️ No se encontró la cámara con el nombre 'camera'.\n");
        wb_robot_cleanup();
        return 1;
    }

    wb_camera_enable(camera, TIME_STEP); // Activar la cámara

    int width = wb_camera_get_width(camera);
    int height = wb_camera_get_height(camera);

    printf("✅ Cámara encontrada: Resolución %dx%d\n", width, height);

    while (wb_robot_step(TIME_STEP) != -1) {
        const unsigned char *image = wb_camera_get_image(camera);
        if (image) {
            printf("📸 Capturando imagen...\n");

            // Crear un nombre de archivo único para la imagen en escala de grises
            char *filename = generate_filename();

            // Crear una imagen en escala de grises
            unsigned char *gray_image = (unsigned char *)malloc(width * height * sizeof(unsigned char));

            // Convertir la imagen a escala de grises
            convert_to_grayscale(image, gray_image, width, height);

            // Guardar la imagen en escala de grises en formato PNG
            if (stbi_write_png(filename, width, height, 1, gray_image, width)) {
                printf("✅ Imagen en escala de grises guardada en: %s\n", filename);
            } else {
                printf("⚠️ Error al guardar la imagen en escala de grises.\n");
            }

            // Liberar la memoria
            free(gray_image);
            free(filename); // Liberar la memoria del nombre de archivo
        }
    }

    wb_robot_cleanup(); // Limpiar Webots
    return 0;
}


