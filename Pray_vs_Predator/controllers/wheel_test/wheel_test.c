#include <webots/robot.h>
#include <webots/motor.h>
#include <stdio.h>

#define TIME_STEP 64  // Paso de simulación en ms
#define SPEED 2.0     // Velocidad de los motores

int main() {
    wb_robot_init(); // Inicializar Webots

    // Obtener los motores
    WbDeviceTag left_motor = wb_robot_get_device("left wheel motor");
    WbDeviceTag right_motor = wb_robot_get_device("right wheel motor");

    if (left_motor == 0 || right_motor == 0) {
        printf("⚠️ Error: No se encontraron los motores.\n");
        wb_robot_cleanup();
        return 1;
    }

    // Configurar los motores en velocidad constante
    wb_motor_set_position(left_motor, INFINITY);
    wb_motor_set_position(right_motor, INFINITY);
    wb_motor_set_velocity(left_motor, SPEED);
    wb_motor_set_velocity(right_motor, SPEED);

    // Mantener la simulación en ejecución
    while (wb_robot_step(TIME_STEP) != -1) {
        // Aquí puedes modificar la velocidad o dirección si lo deseas
    }

    wb_robot_cleanup(); // Limpiar Webots
    return 0;
}
