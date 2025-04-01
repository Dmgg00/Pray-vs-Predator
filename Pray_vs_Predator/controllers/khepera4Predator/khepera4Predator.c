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

#define MAX_SPEED 47.6

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define NUMBER_OF_ULTRASONIC_SENSORS 5
static const char *ultrasonic_sensors_names[NUMBER_OF_ULTRASONIC_SENSORS] = {
  "left ultrasonic sensor", "front left ultrasonic sensor", "front ultrasonic sensor", "front right ultrasonic sensor",
  "right ultrasonic sensor"};

#define NUMBER_OF_INFRARED_SENSORS 12
static const char *infrared_sensors_names[NUMBER_OF_INFRARED_SENSORS] = {
  // turret sensors
  "rear left infrared sensor", "left infrared sensor", "front left infrared sensor", "front infrared sensor",
  "front right infrared sensor", "right infrared sensor", "rear right infrared sensor", "rear infrared sensor",
  // ground sensors
  "ground left infrared sensor", "ground front left infrared sensor", "ground front right infrared sensor",
  "ground right infrared sensor"};
  
char* generate_filename() {
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);

    // Obtener la ruta de HOME
    const char *home = getenv("HOME");
    if (home == NULL) {
        fprintf(stderr, "Error: No se pudo obtener la variable HOME.\n");
        return NULL;
    }

    // Tamaño suficiente para la ruta completa
    size_t max_size = strlen(home) + strlen("/Pray-vs-Predator/Pray_vs_Predator/images/Predator/khepera_predator_YYYYMMDD_HHMMSS.png") + 1;
    char *filepath = (char *)malloc(max_size);
    if (filepath == NULL) {
        fprintf(stderr, "Error: No se pudo asignar memoria para el filename.\n");
        return NULL;
    }

    // Construir la ruta base
    snprintf(filepath, max_size, "%s/Pray-vs-Predator/Pray_vs_Predator/images/Predator/", home);

    // Generar el nombre con la fecha y hora
    char timestamp[50];  // Espacio suficiente para la fecha y hora
    strftime(timestamp, sizeof(timestamp), "khepera_predator_%Y%m%d_%H%M%S.png", tm_info);

    // Concatenar la ruta con el nombre del archivo
    strncat(filepath, timestamp, max_size - strlen(filepath) - 1);

    return filepath;  // Retorna la ruta completa del archivo
}

int main(int argc, char **argv) {
  wb_robot_init();

  int time_step = (int)wb_robot_get_basic_time_step();
  int i;

  // get and enable the camera
  WbDeviceTag camera = wb_robot_get_device("camera");
  wb_camera_enable(camera, time_step);
  
  int width = wb_camera_get_width(camera);
  int height = wb_camera_get_height(camera);

  // get and enable the ultrasonic sensors
  WbDeviceTag ultrasonic_sensors[5];
  for (i = 0; i < 5; ++i) {
    ultrasonic_sensors[i] = wb_robot_get_device(ultrasonic_sensors_names[i]);
    wb_distance_sensor_enable(ultrasonic_sensors[i], time_step);
  }

  // get and enable the infrared sensors
  WbDeviceTag infrared_sensors[12];
  for (i = 0; i < 12; ++i) {
    infrared_sensors[i] = wb_robot_get_device(infrared_sensors_names[i]);
    wb_distance_sensor_enable(infrared_sensors[i], time_step);
  }

  // get the led actuators
  WbDeviceTag leds[3] = {wb_robot_get_device("front left led"), wb_robot_get_device("front right led"),
                         wb_robot_get_device("rear led")};

  // get the motors and set target position to infinity (speed control)
  WbDeviceTag left_motor, right_motor;
  left_motor = wb_robot_get_device("left wheel motor");
  right_motor = wb_robot_get_device("right wheel motor");
  wb_motor_set_position(left_motor, INFINITY);
  wb_motor_set_position(right_motor, INFINITY);
  wb_motor_set_velocity(left_motor, 0.0);
  wb_motor_set_velocity(right_motor, 0.0);

  // store the last time a message was displayed
  int last_display_second = 0;
  
  unsigned char *rgb_image = (unsigned char *)malloc(3 * width * height * sizeof(unsigned char));

  // main loop
  while (wb_robot_step(time_step) != -1) {
    // display some sensor data every second
    // and change randomly the led colors
    int display_second = (int)wb_robot_get_time();
    if (display_second != last_display_second) {
      last_display_second = display_second;

      printf("time = %d [s]\n", display_second);
      for (i = 0; i < 5; ++i)
        printf("- ultrasonic sensor('%s') = %f [m]\n", ultrasonic_sensors_names[i],
               wb_distance_sensor_get_value(ultrasonic_sensors[i]));
      for (i = 0; i < 12; ++i)
        printf("- infrared sensor('%s') = %f [m]\n", infrared_sensors_names[i],
               wb_distance_sensor_get_value(infrared_sensors[i]));

      for (i = 0; i < 3; ++i){
        wb_led_set(leds[i], 0xFFFFFF & rand());
        }
        
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

    // simple obstacle avoidance algorithm
    // based on the front infrared sensors
    double speed_offset = 0.2 * (MAX_SPEED - 0.03 * wb_distance_sensor_get_value(infrared_sensors[3]));
    double speed_delta =
      0.03 * wb_distance_sensor_get_value(infrared_sensors[2]) - 0.03 * wb_distance_sensor_get_value(infrared_sensors[4]);
    wb_motor_set_velocity(left_motor, speed_offset + speed_delta);
    wb_motor_set_velocity(right_motor, speed_offset - speed_delta);
  };

  wb_robot_cleanup();

  return EXIT_SUCCESS;
}
