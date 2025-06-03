#include "pico/stdlib.h"      
#include "pico/cyw43_arch.h"  
#include "hardware/adc.h" 

#include "setup.h"
#include "inc/ssd1306.h"

#ifndef DEBUG_printf
#ifndef NDEBUG
#define DEBUG_printf printf
#else
#define DEBUG_printf(...)
#endif
#endif

#ifndef INFO_printf
#define INFO_printf printf
#endif

#ifndef ERROR_printf
#define ERROR_printf printf
#endif

// Implementação das funções de inicialização 
void gpio_led_bitdog(void) {
    gpio_init(LED_BLUE_PIN);
    gpio_set_dir(LED_BLUE_PIN, GPIO_OUT);
    gpio_put(LED_BLUE_PIN, false);

    gpio_init(LED_GREEN_PIN);
    gpio_set_dir(LED_GREEN_PIN, GPIO_OUT);
    gpio_put(LED_GREEN_PIN, false);

    gpio_init(LED_RED_PIN);
    gpio_set_dir(LED_RED_PIN, GPIO_OUT);
    gpio_put(LED_RED_PIN, false);
}

void iniciar_display(){
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    ssd1306_init(&ssd, WIDTH, HEIGHT, false, endereco, I2C_PORT);
    ssd1306_config(&ssd);
    ssd1306_send_data(&ssd);
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);
}

// Requisição para publicar
void pub_request_cb(void *arg, err_t err) { 
    if (err != 0) {
        ERROR_printf("pub_request_cb failed %d\n", err); 
    }
}

//Topico MQTT
const char *full_topic(MQTT_CLIENT_DATA_T *state, const char *name) { 
#if MQTT_UNIQUE_TOPIC
    static char full_topic_buf[MQTT_TOPIC_LEN]; 
    snprintf(full_topic_buf, sizeof(full_topic_buf), "/%s%s", state->mqtt_client_info.client_id, name);
    return full_topic_buf;
#else
    return name;
#endif
}

// Requisição de Assinatura - subscribe
void sub_request_cb(void *arg, err_t err) { 
    MQTT_CLIENT_DATA_T* state_data = (MQTT_CLIENT_DATA_T*)arg; 
    if (err != 0) {
        panic("subscribe request failed %d", err);
    }
    state_data->subscribe_count++;
}

// Requisição para encerrar a assinatura
void unsub_request_cb(void *arg, err_t err) { 
    MQTT_CLIENT_DATA_T* state_data = (MQTT_CLIENT_DATA_T*)arg; 
    if (err != 0) {
        panic("unsubscribe request failed %d", err);
    }
    state_data->subscribe_count--;
    assert(state_data->subscribe_count >= 0);

    if (state_data->subscribe_count <= 0 && state_data->stop_client) {
        mqtt_disconnect(state_data->mqtt_client_inst);
    }
}

// Tópicos de assinatura - SIMPLIFICADOS
void sub_unsub_topics(MQTT_CLIENT_DATA_T* state_param, bool sub) { 
    mqtt_request_cb_t cb = sub ? sub_request_cb : unsub_request_cb;

    mqtt_sub_unsub(state_param->mqtt_client_inst, "/modo", MQTT_SUBSCRIBE_QOS, cb, state_param, sub);
    mqtt_sub_unsub(state_param->mqtt_client_inst, "/alarme", MQTT_SUBSCRIBE_QOS, cb, state_param, sub);
    mqtt_sub_unsub(state_param->mqtt_client_inst, full_topic(state_param, "/print"), MQTT_SUBSCRIBE_QOS, cb, state_param, sub);
    mqtt_sub_unsub(state_param->mqtt_client_inst, full_topic(state_param, "/ping"), MQTT_SUBSCRIBE_QOS, cb, state_param, sub);
    mqtt_sub_unsub(state_param->mqtt_client_inst, full_topic(state_param, "/exit"), MQTT_SUBSCRIBE_QOS, cb, state_param, sub);
}

// Dados de entrada MQTT - SIMPLIFICADOS
void mqtt_incoming_data_cb(void *arg, const u8_t *data, u16_t len, u8_t flags) { 
    MQTT_CLIENT_DATA_T* state_data = (MQTT_CLIENT_DATA_T*)arg; 

    char msg[len + 1];
    memcpy(msg, data, len);
    msg[len] = '\0';

    INFO_printf("Mensagem recebida no topico: %s, Payload: %s\n", state_data->topic, msg);

    if (strcmp(state_data->topic, full_topic(state_data, "/alarme")) == 0) {
        if (strncmp(msg, "desativar", len) == 0 && state_data->alarme_ativo) {
            state_data->alarme_ativo = false;
            INFO_printf(">> Comando MQTT: Alarme DESATIVADO\n");
            buzzer_desliga(BUZZER_PIN);

            atualiza_display(state_data); 
            atualiza_matriz_leds(state_data);

            publish_alarm_status(state_data);
        } else {
            INFO_printf(">> Comando /alarme ignorado (já no estado desejado, ou não permitido)\n");
        }
    } else if (strcmp(state_data->topic, full_topic(state_data, "/modo")) == 0) {
        ModoSistema novo_modo_solicitado = state_data->modo_atual;
        if (strcmp(msg, "home") == 0) novo_modo_solicitado = MODO_HOME;
        else if (strcmp(msg, "festa") == 0) novo_modo_solicitado = MODO_FESTA;
        else if (strcmp(msg, "seguranca") == 0) novo_modo_solicitado = MODO_SEGURANCA;
        else if (strcmp(msg, "leds") == 0) alternar_leds(state_data);

        if (state_data->modo_atual != novo_modo_solicitado) {
            set_modo(state_data, novo_modo_solicitado); 
        }
    } else {
        INFO_printf(">> Comando não reconhecido\n");
    }
}

// Dados de entrada publicaddos
void mqtt_incoming_publish_cb(void *arg, const char *topic, u32_t tot_len) { 
    MQTT_CLIENT_DATA_T* state_data = (MQTT_CLIENT_DATA_T*)arg; 
    strncpy(state_data->topic, topic, sizeof(state_data->topic) -1 ); 
    state_data->topic[sizeof(state_data->topic) - 1] = '\0'; 
}

// Função para publicar o status do modo
void publish_mode_status(MQTT_CLIENT_DATA_T *state_param) { 
    char topic_buf[MQTT_TOPIC_LEN];

    snprintf(topic_buf, sizeof(topic_buf), "%s/status/modo", state_param->mqtt_client_info.client_id);

    char msg_buf[20];
    switch (state_param->modo_atual) {
        case MODO_HOME: strcpy(msg_buf, "home"); break;
        case MODO_FESTA: strcpy(msg_buf, "festa"); break;
        case MODO_SEGURANCA: strcpy(msg_buf, "seguranca"); break;
        default: strcpy(msg_buf, "desconhecido"); break;
    }
    mqtt_publish(state_param->mqtt_client_inst, topic_buf, msg_buf, strlen(msg_buf), MQTT_PUBLISH_QOS, MQTT_PUBLISH_RETAIN, pub_request_cb, state_param);
    INFO_printf("Publicado modo: %s em %s\n", msg_buf, topic_buf);
}

// Função para publicar o status do alarme
void publish_alarm_status(MQTT_CLIENT_DATA_T *state_param) { 
    char topic_buf[MQTT_TOPIC_LEN];
    snprintf(topic_buf, sizeof(topic_buf), "%s/status/alarme", state_param->mqtt_client_info.client_id);

    char *msg = state_param->alarme_ativo ? "intruso" : "ok";

    mqtt_publish(state_param->mqtt_client_inst, topic_buf, msg, strlen(msg), MQTT_PUBLISH_QOS, MQTT_PUBLISH_RETAIN, pub_request_cb, state_param);
    INFO_printf("Publicado status alarme: %s em %s\n", msg, topic_buf);
}

// Conexão MQTT
 void mqtt_connection_cb(mqtt_client_t *client, void *arg, mqtt_connection_status_t status) { 
    MQTT_CLIENT_DATA_T* state_data = (MQTT_CLIENT_DATA_T*)arg; 
    if (status == MQTT_CONNECT_ACCEPTED) {
        state_data->connect_done = true;
        sub_unsub_topics(state_data, true);

        if (state_data->mqtt_client_info.will_topic) {
            mqtt_publish(state_data->mqtt_client_inst, state_data->mqtt_client_info.will_topic, "1", 1, MQTT_WILL_QOS, true, pub_request_cb, state_data);
        }
        INFO_printf("MQTT connected successfully!\n"); 
    } else if (status == MQTT_CONNECT_DISCONNECTED) {
        ERROR_printf("MQTT disconnected. Status: %d\n", status); 
        if (!state_data->connect_done) { 
            panic("Failed to connect to mqtt server initially");
        }
    } else {
        ERROR_printf("Unexpected MQTT connection status: %d\n", status);
    }
}

// Inicializar o cliente MQTT
void start_client(MQTT_CLIENT_DATA_T *state_param) { 
#if LWIP_ALTCP && LWIP_ALTCP_TLS
    const int port = MQTT_TLS_PORT; 
    INFO_printf("Using TLS on port %d\n", port);
#else
    const int port = MQTT_PORT; 
    INFO_printf("Warning: Not using TLS. Using port %d\n", port);
#endif

    state_param->mqtt_client_inst = mqtt_client_new();
    if (!state_param->mqtt_client_inst) {
        panic("MQTT client instance creation error");
    }
    INFO_printf("Connecting to mqtt server at %s\n", ipaddr_ntoa(&state_param->mqtt_server_address));

    cyw43_arch_lwip_begin(); 
    err_t err = mqtt_client_connect(state_param->mqtt_client_inst, &state_param->mqtt_server_address, port, mqtt_connection_cb, state_param, &state_param->mqtt_client_info);
    if (err != ERR_OK) {
        ERROR_printf("MQTT broker connection error: %d\n", err);
        panic("MQTT broker connection error");
    }
#if LWIP_ALTCP && LWIP_ALTCP_TLS && defined(MQTT_SERVER)
    // mbedtls_ssl_set_hostname precisa de MQTT_SERVER (o nome do host, não o IP)
    // Se MQTT_SERVER é um IP, esta chamada pode não ser necessária ou pode precisar do IP como string.
    // Verifique a documentação de mbedtls_ssl_set_hostname.
    // Assumindo que MQTT_SERVER é o hostname definido no main.c (ex: "192.168.0.102")
    // Se MQTT_SERVER for um IP, esta linha pode causar problemas ou ser ineficaz.
    // Para SNI, o nome do host real é necessário.
    // extern const char* MQTT_SERVER_HOST; // Se você tiver o hostname em uma variável global
    // mbedtls_ssl_set_hostname(altcp_tls_context(state_param->mqtt_client_inst->conn), MQTT_SERVER_HOST);
#endif
    mqtt_set_inpub_callback(state_param->mqtt_client_inst, mqtt_incoming_publish_cb, mqtt_incoming_data_cb, state_param);
    cyw43_arch_lwip_end();
}

// Call back com o resultado do DNS
void dns_found(const char *hostname, const ip_addr_t *ipaddr, void *arg) {
    MQTT_CLIENT_DATA_T *state_data = (MQTT_CLIENT_DATA_T*)arg;
    if (ipaddr) {
        state_data->mqtt_server_address = *ipaddr;
        INFO_printf("DNS resolved for %s: %s\n", hostname, ipaddr_ntoa(ipaddr)); 
        start_client(state_data);
    } else {
        ERROR_printf("DNS request failed for %s\n", hostname); 
        panic("dns request failed");
    }
}