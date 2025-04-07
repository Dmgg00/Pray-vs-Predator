/*
 * Copyright 1996-2024 Cyberbotics Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     https://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <webots/motor.h>
#include <webots/robot.h>

#include <webots/camera.h>
#include <webots/distance_sensor.h>
#include <webots/led.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../op_matrices.h"
#define MAX_SPEED 47.6

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
  
int contador = 0;

char* generate_filename() {
    const char *home = getenv("HOME");
    if (home == NULL) {
        fprintf(stderr, "Error: No se pudo obtener la variable HOME.\n");
        return NULL;
    }

    size_t max_size = strlen(home) + strlen("/Pray-vs-Predator/Pray_vs_Predator/images/Predator/khepera_predator_#.png") + 1;
    char *filepath = (char *)malloc(max_size);
    if (filepath == NULL) {
        fprintf(stderr, "Error: No se pudo asignar memoria para el filename.\n");
        return NULL;
    }

    snprintf(filepath, max_size, "%s/Pray-vs-Predator/Pray_vs_Predator/images/Predator/khepera_predator_", home);
    snprintf(filepath + strlen(filepath), max_size - strlen(filepath), "%d.png", contador);

    contador++;
    return filepath;
}


int main(int argc, char **argv) {
  wb_robot_init();

  int time_step = (int)wb_robot_get_basic_time_step();

  // Obtener y habilitar la cámara
  WbDeviceTag camera = wb_robot_get_device("camera");
  wb_camera_enable(camera, time_step);
  
  int width = wb_camera_get_width(camera);
  int height = wb_camera_get_height(camera);
  
  int last_display_second = 0;
    
  // Asignar memoria para las imágenes (20 filas en vez de todo el alto)
  uint8_t* imagen_gris = malloc(width * 20);  // Solo un byte por píxel (escala de grises)
  uint8_t* imagen_azul = malloc(width * 20);  // Solo un byte por píxel (canal azul)

  while (wb_robot_step(time_step) != -1) {
    int display_second = (int)wb_robot_get_time();
    if (display_second != last_display_second) {
      last_display_second = display_second;
      printf("time = %d [s]\n", display_second);
        
      const unsigned char *image = wb_camera_get_image(camera);

      if (image) {
        int filas_a_extraer = 20;
        int fila_inicio = (height - filas_a_extraer) / 2;
        
        // Extraer las 20 filas centrales
        for (int y = 0; y < filas_a_extraer; y++) {
          int fila_real = fila_inicio + y;
          for (int x = 0; x < width; x++) {
            int index = 4 * (fila_real * width + x);  // BGRA
            unsigned char B = image[index + 0];
            unsigned char G = image[index + 1];
            unsigned char R = image[index + 2];

            // Convertir a gris usando fórmula estándar de luminancia
            uint8_t gris = (uint8_t)(0.114 * B + 0.587 * G + 0.299 * R);
            imagen_gris[y * width + x] = gris;
        
         if (R > 200 && G > 200 && B > 200) {
            // Este píxel es blanco, no lo tomamos
            imagen_azul[y * width + x] = 0;  // O asignar un valor que no se vea en la imagen
        } else {
            // Aplicar threshold al componente azul
            int threshold = 100;  // Ajusta este valor según lo que consideres como umbral
            if (B > threshold) {
                imagen_azul[y * width + x] = B;  // Guardar el valor del canal azul si es mayor que el umbral
            } else {
                imagen_azul[y * width + x] = 0;  // Si no, asignar 0 (sin valor visible)
            }
        }
          }
        }

        // Guardar imagen gris
        char *filename_gris = generate_filename();
        if (stbi_write_png(filename_gris, width, filas_a_extraer, 1, imagen_gris, width)) {
          printf("Imagen gris guardada en: %s\n", filename_gris);
        } else {
          printf("Error al guardar la imagen gris.\n");
        }

        // Guardar imagen con solo canal azul
        char *filename_azul = generate_filename();
        if (stbi_write_png(filename_azul, width, filas_a_extraer, 1, imagen_azul, width)) {
          printf("Imagen azul guardada en: %s\n", filename_azul);
        } else {
          printf("Error al guardar la imagen azul.\n");
        }

        free(filename_gris);
        free(filename_azul);
      }
    }
  }

  // Liberar memoria de las imágenes al final
  free(imagen_gris);
  free(imagen_azul);

  wb_robot_cleanup();

  return EXIT_SUCCESS;
}