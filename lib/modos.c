//Bibliotecas de apoio
#include "inc/buzzer.h"
#include "inc/matriz_led.h"
#include "inc/ssd1306.h"
#include "inc/modos.h"
#include "hardware/adc.h"
#include "setup.h"
#include <stdio.h>               
#include <string.h>              
#include <stdlib.h>   

ssd1306_t ssd;


//Executa os loops do modos
void executar_modulo_modos(MQTT_CLIENT_DATA_T *state) {
    if (state->modo_atual == MODO_SEGURANCA && !state->alarme_ativo && detect_loud_noise()) {
        printf("Alarme ativado por ruído!\n");
        state->alarme_ativo = true;
        
        atualiza_display(state);  // Para resposta imediata
        atualiza_matriz_leds(state); // Para resposta imediata
        publish_alarm_status(state); // Publicar que há um intruso
    }
    alarme_loop(state);         
    animacao_festa_loop(state); 
    musica_festa_loop(state);   

    sleep_ms(100);
}

//Define o modo atual no sistema
void set_modo(MQTT_CLIENT_DATA_T *state, ModoSistema novo_modo) { 
    state->modo_atual = novo_modo;

    switch (state->modo_atual) {
        case MODO_HOME:
            printf("Modo atual: Home\n");
            break;
        case MODO_FESTA:
            printf("Modo atual: Festa\n");
            break;
        case MODO_SEGURANCA:
            printf("Modo atual: Segurança\n");
            break;
    }

    atualiza_display(state);
    atualiza_matriz_leds(state);
    atualiza_buzzer(state); 
    atualiza_rgb_led(state); 

    publish_mode_status(state);
    publish_alarm_status(state);

}

// ===========Funções de periféricos=================
//Atualiza o buzzer de acordo com o modo
void atualiza_buzzer(MQTT_CLIENT_DATA_T *state) {
    if (state-> alarme_ativo) {
    } else {
        buzzer_desliga(BUZZER_PIN);
        if (state->modo_atual == MODO_FESTA) {
            tocar_frequencia(440, 150);
        }
    }
}

//Atualiza o display de acordo com o modo
void atualiza_display(MQTT_CLIENT_DATA_T *state) {
    ssd1306_fill(&ssd, false);

    char buffer_data[16];
    char buffer_hora[16];
    // Se o modo for Segurança com alarme, mostrar "INTRUSO"
    if (state->alarme_ativo) {
        ssd1306_draw_string(&ssd, "!!! INTRUSO !!!", 5, 30);
        ssd1306_send_data(&ssd);
        return;
    }

    // Nome do modo
    switch (state->modo_atual) {
        case MODO_HOME:
            ssd1306_draw_string(&ssd, "Modo: Home", 28, 24);
            break;
        case MODO_FESTA:
            ssd1306_draw_string(&ssd, "Modo: Festa", 28, 20);
            break;
        case MODO_SEGURANCA:
            ssd1306_draw_string(&ssd, "Modo: Seguranca", 28, 2);
            break;
        default:
            break;
    }

    ssd1306_send_data(&ssd);
}

//Atualizar matriz de LED de acordo com o modo
void atualiza_matriz_leds(MQTT_CLIENT_DATA_T *state) {
    switch (state->modo_atual) {
        case MODO_HOME:
            exibir_padrao(0);
            break;
        case MODO_FESTA:
            exibir_padrao(1); 
            break;
        case MODO_SEGURANCA:
            if (state->alarme_ativo) {
                piscar_matriz_intruso(); 
            } else {
            clear_matrix(pio0, 0);
            update_leds(pio0, 0);
        }
        break;
    }
}

//Atualizar LED RGB de acordo com modo
void atualiza_rgb_led(MQTT_CLIENT_DATA_T *state) {
    switch (state->modo_atual) {
        case MODO_HOME:
            gpio_put(LED_GREEN_PIN, true);
            gpio_put(LED_RED_PIN, false);
            gpio_put(LED_BLUE_PIN, false);
            break;
        case MODO_FESTA:
            gpio_put(LED_GREEN_PIN, false);
            gpio_put(LED_RED_PIN, true);
            gpio_put(LED_BLUE_PIN, true);
            break;
        case MODO_SEGURANCA:
            gpio_put(LED_GREEN_PIN, false);
            gpio_put(LED_RED_PIN, true);
            gpio_put(LED_BLUE_PIN, false);
            break;
    }
}

bool detect_loud_noise(void) {
    uint32_t sum = 0;
    uint16_t max_value = 0;
    uint16_t sample;

    adc_select_input(2);  // Canal onde o microfone está

    for (int i = 0; i < MIC_SAMPLES; i++) {
        sample = adc_read();
        if (sample > max_value) max_value = sample;
        sum += sample;
        sleep_us(100);
    }

    return (max_value > MIC_THRESHOLD);
}
