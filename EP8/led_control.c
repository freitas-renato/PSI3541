#include "led_control.h"
#include "sensor.h"

#include "pthread.h"
#include  <stdlib.h>
#include <unistd.h>

pthread_t led_control_thread;

static led_t main_led;
static int manual_intensity;


void* led_controller(void* arg) {
    for (;;) {
        switch (main_led.op_mode) {
            case STANDBY:
                main_led.intensity = 0;
            break;

            case MANUAL:
                main_led.intensity = manual_intensity;
            
            break;

            case AUTO:
                main_led.intensity = get_valor_sensor();
            break;
        }

        sleep(1);
    }
}


void init_led_controller(void) {
    pthread_create(&led_control_thread, NULL, led_controller, NULL);
}


op_modes_t get_op_mode(void) {
    return main_led.op_mode;
}

void set_op_mode(op_modes_t mode) {
    main_led.op_mode = mode;
}

int get_led_intensity(void) {
    return main_led.intensity;
}


// Usado pra setar manualmente a intensidade, ignorado no modo automatico
void set_led_intensity(int intensity) {
    manual_intensity = intensity;
}
