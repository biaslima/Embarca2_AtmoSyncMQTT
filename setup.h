#ifndef SETUP_H
#define SETUP_H

#include "lwip/apps/mqtt.h"         
#include "lwip/apps/mqtt_priv.h"    
#include "lwip/dns.h"               
#include "lwip/altcp_tls.h"  

#ifndef MQTT_TOPIC_LEN
#define MQTT_TOPIC_LEN 100
#endif

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

// Tipos de modo do sistema
typedef enum {
    MODO_HOME,
    MODO_FESTA,
    MODO_SEGURANCA,
} ModoSistema;

//Dados do cliente MQTT
typedef struct {
    mqtt_client_t* mqtt_client_inst;
    struct mqtt_connect_client_info_t mqtt_client_info;
    char data[MQTT_OUTPUT_RINGBUF_SIZE];
    char topic[MQTT_TOPIC_LEN];
    uint32_t len;
    ip_addr_t mqtt_server_address;
    bool connect_done;
    int subscribe_count;
    bool stop_client;

    ModoSistema modo_atual;       // Standby, Festa ou Segurança
    bool alarme_ativo;            // Alarme ativo ou não   
} MQTT_CLIENT_DATA_T;


#include "inc/ssd1306.h"
#include "inc/matriz_led.h"
#include "inc/buzzer.h"
#include "inc/modos.h"

//Variável global do display
extern ssd1306_t ssd;

void gpio_led_bitdog(void);
void iniciar_display(void); 
void publish_mode_status(MQTT_CLIENT_DATA_T *state);
void publish_alarm_status(MQTT_CLIENT_DATA_T *state);

#endif