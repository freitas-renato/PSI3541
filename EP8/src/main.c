#include "sensor.h"
#include "led_control.h"
#include "tcp_server.h"


// #include <pthread.h>

int main(void) {

    init_sensor();
    init_led_controller();

    // pthread_t *t = init_server();
    init_server();

    // pthread_join(t, NULL);

    return 0;
}