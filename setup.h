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

// Manter o programa ativo - keep alive in seconds
#define MQTT_KEEP_ALIVE_S 60

// QoS - mqtt_subscribe
// At most once (QoS 0)
// At least once (QoS 1)
// Exactly once (QoS 2)
#define MQTT_SUBSCRIBE_QOS 1
#define MQTT_PUBLISH_QOS 1
#define MQTT_PUBLISH_RETAIN 0

// Tópico usado para: last will and testament
#define MQTT_WILL_TOPIC "/online"
#define MQTT_WILL_MSG "0"
#define MQTT_WILL_QOS 1

#ifndef MQTT_DEVICE_NAME
#define MQTT_DEVICE_NAME "pico"
#endif

// Definir como 1 para adicionar o nome do cliente aos tópicos, para suportar vários dispositivos que utilizam o mesmo servidor
#ifndef MQTT_UNIQUE_TOPIC
#define MQTT_UNIQUE_TOPIC 0
#endif

#include "inc/ssd1306.h"
#include "inc/matriz_led.h"
#include "inc/buzzer.h"
#include "inc/modos.h"

//Variável global do display
extern ssd1306_t ssd;

// Funções de inicialização e GPIO
void gpio_led_bitdog(void);
void iniciar_display(void); 

void pub_request_cb(__unused void *arg, err_t err); // Requisição para publicar
const char *full_topic(MQTT_CLIENT_DATA_T *state, const char *name);// Topico MQTT
void sub_request_cb(void *arg, err_t err);// Requisição de Assinatura - subscribe
void unsub_request_cb(void *arg, err_t err);// Requisição para encerrar a assinatura
void sub_unsub_topics(MQTT_CLIENT_DATA_T* state, bool sub);// Tópicos de assinatura
void mqtt_incoming_data_cb(void *arg, const u8_t *data, u16_t len, u8_t flags);// Dados de entrada MQTT
void mqtt_incoming_publish_cb(void *arg, const char *topic, u32_t tot_len);// Dados de entrada publicados
void mqtt_connection_cb(mqtt_client_t *client, void *arg, mqtt_connection_status_t status);// Conexão MQTT
void start_client(MQTT_CLIENT_DATA_T *state);// Inicializar o cliente MQTT
void dns_found(const char *hostname, const ip_addr_t *ipaddr, void *arg);// Call back com o resultado do DNS

// Funções de publicação de status
void publish_mode_status(MQTT_CLIENT_DATA_T *state);
void publish_alarm_status(MQTT_CLIENT_DATA_T *state);

#endif