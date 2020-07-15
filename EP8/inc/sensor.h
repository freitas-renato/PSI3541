// arquivo: sensor.h
// Incluir este arquivo no programa principal

# ifndef __SENSOR_H__
# define __SENSOR_H__

#define  PERIODO  2   // Periodo (em segundos) no qual eh alterado o valor 

void init_sensor(void);
int get_valor_sensor(void);


#endif  // __SENSOR_H__