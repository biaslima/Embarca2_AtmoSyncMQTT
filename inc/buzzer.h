#ifndef BUZZER_H
#define BUZZER_H

#include "pico/stdlib.h"
#include "setup.h" 

void buzzer_init(int pin);
void tocar_frequencia(int frequencia, int duracao_ms);
void buzzer_desliga(int pin);
void buzzer_liga(int pin);
void alarme_loop(MQTT_CLIENT_DATA_T *state);
void musica_festa_loop(MQTT_CLIENT_DATA_T *state);

#endif