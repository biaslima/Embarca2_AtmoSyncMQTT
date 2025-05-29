#ifndef SETUP_H
#define SETUP_H

#include "inc/ssd1306.h"
#include "inc/matriz_led.h"
#include "inc/buzzer.h"
#include "inc/modos.h"

// Definição dos pinos dos LEDs
#define LED_PIN CYW43_WL_GPIO_LED_PIN  
#define LED_BLUE_PIN 12                 
#define LED_GREEN_PIN 11                
#define LED_RED_PIN 13
#define BUZZER_PIN 21           
#define LED_MATRIX_PIN 7

#define MIC_THRESHOLD 2200
#define MIC_SAMPLES 100

//VARIÁVEIS DO DISPLAY
#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define endereco 0x3C

//Variável global do display
extern ssd1306_t ssd;

void gpio_led_bitdog(void);
void iniciar_display(void); 

#endif