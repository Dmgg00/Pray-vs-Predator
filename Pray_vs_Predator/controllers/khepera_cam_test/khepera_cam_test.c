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
      strftime(filename, 100, "/home/mapache/Pictures/pr_py_test/khepera_%Y%m%d_%H%M%S.png", tm_info);
    
    return filename;
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

    // Buffer para almacenar la imagen convertida de BGRA a RGB
    unsigned char *rgb_image = (unsigned char *)malloc(3 * width * height * sizeof(unsigned char));

    while (wb_robot_step(TIME_STEP) != -1) {
        const unsigned char *image = wb_camera_get_image(camera);
        if (image) {
            printf("📸 Capturando imagen...\n");

            // Convertir de BGRA a RGB (Webots devuelve BGRA)
            for (int y = 0; y < height; y++) {
                for (int x = 0; x < width; x++) {
                    int index = (y * width + x) * 4;  // BGRA usa 4 bytes por píxel
                    int new_index = (y * width + x) * 3; // RGB usa 3 bytes por píxel
                    rgb_image[new_index] = image[index + 2];     // Rojo
                    rgb_image[new_index + 1] = image[index + 1]; // Verde
                    rgb_image[new_index + 2] = image[index];     // Azul
                }
            }

            // Crear un nombre de archivo único
            char *filename = generate_filename();

            // Guardar la imagen en formato PNG
            if (stbi_write_png(filename, width, height, 3, rgb_image, width * 3)) {
                printf("✅ Imagen guardada en: %s\n", filename);
            } else {
                printf("⚠️ Error al guardar la imagen.\n");
            }

            free(filename); // Liberar la memoria del nombre de archivo
        }
    }

    free(rgb_image); // Liberar la memoria del buffer de imagen
    wb_robot_cleanup(); // Limpiar Webots
    return 0;
}
