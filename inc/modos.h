#ifndef MODOS_H
#define MODOS_H

#include "setup.h" 

// Protótipos das funções
void set_modo(MQTT_CLIENT_DATA_T *state, ModoSistema novo_modo);
void atualiza_display(MQTT_CLIENT_DATA_T *state);
void atualiza_matriz_leds(MQTT_CLIENT_DATA_T *state);
void atualiza_buzzer(MQTT_CLIENT_DATA_T *state);
void atualiza_rgb_led(MQTT_CLIENT_DATA_T *state);
void executar_modulo_modos(MQTT_CLIENT_DATA_T *state);
bool detect_loud_noise(void);

#endif