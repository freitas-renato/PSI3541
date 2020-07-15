#ifndef __LED_CONTROL_H__
#define __LED_CONTROL_H__

typedef enum op_modes {
    STANDBY = 0,
    MANUAL,
    AUTO
} op_modes_t;


typedef struct {
    op_modes_t op_mode;
    int intensity;
} led_t;


void init_led_controller(void);

op_modes_t get_op_mode(void);
void set_op_mode(op_modes_t mode);

int get_led_intensity(void);
void set_led_intensity(int intensity);

int get_ambient_luminosity(void);

#endif  // __LED_CONTROL_H__
