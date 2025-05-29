#include "setup.h"
#include "ws2812.pio.h"
#include <stdio.h>               
#include <string.h>              
#include <stdlib.h>  
#include "pico/time.h"

uint32_t leds[NUM_LEDS];
PIO pio = pio0;
int sm = 0;
bool leds_ativos = true; 

//Inicializa a matriz
void matrix_init(PIO pio_inst, uint sm_num, uint pin) {
    pio = pio_inst;
    sm = sm_num;
    
    gpio_init(pin);
    gpio_set_dir(pin, GPIO_OUT);
    
    uint offset = pio_add_program(pio, &ws2812_program);
    printf("Loaded PIO program at offset %d\n", offset);
    ws2812_program_init(pio, sm, offset, pin, 800000, IS_RGBW);

    for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = 0; 
    }
    clear_matrix(pio, sm);
    update_leds(pio, sm);
    
    printf("Matriz de LEDs inicializada com sucesso!\n");
}

//Função pra localizar LEDs 
uint8_t matriz_posicao_xy(uint8_t x, uint8_t y) {
    if (y % 2 == 0) {
        return y * 5 + x;
    } else {
        return y * 5 + (4 - x);
    }
}

// Função para criar uma cor
uint32_t create_color(uint8_t green, uint8_t red, uint8_t blue) {
    return ((uint32_t)green << 16) | ((uint32_t)red << 8) | blue;
}

// Limpa todos os LEDs
void clear_matrix(PIO pio_inst, uint sm_num) {
    for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = 0;
    }
}

//Atauliza os LEDs
void update_leds(PIO pio_inst, uint sm_num) {
    for (int i = 0; i < NUM_LEDS; i++) {
        pio_sm_put_blocking(pio_inst, sm_num, leds[i] << 8u);
    }
}

//Cria animação aleatória para modo festa
void animacao_festa_loop() {
    static uint32_t last_time = 0;
    uint32_t current_time = to_ms_since_boot(get_absolute_time());

    if (!leds_ativos || modo_atual != MODO_FESTA) return;

    // Atualiza a cada 300ms
    if (current_time - last_time >= 300) {
        for (int i = 0; i < NUM_LEDS; i++) {
            uint8_t r = rand() % 80;
            uint8_t g = rand() % 80;
            uint8_t b = rand() % 80;
            leds[i] = create_color(g, r, b);
        }
        update_leds(pio, sm);
        last_time = current_time;
    }
}

// Exibe padrão para modo específico
void exibir_padrao(uint8_t padrao) {
    clear_matrix(pio, sm);
    
    switch(padrao) {
        case 0: // Modo Conforto 
            for (int i = 0; i < NUM_LEDS; i++) {
                leds[i] = create_color(7, 20, 3); // Luz amarelada
            }
            break;
        case 2: // Modo Segurança (vermelho)
            for (int i = 0; i < NUM_LEDS; i++) {
                leds[i] = create_color(0, 64, 0); // Vermelho
            }
            break;

        default:
            break;
    }
    
    update_leds(pio, sm);
}

//Alterna a matriz de LEDs
void alternar_leds() {
    leds_ativos = !leds_ativos;

    if (leds_ativos) {
        extern ModoSistema modo_atual; 
        
        uint8_t padrao;
        switch (modo_atual) {
            case MODO_HOME:
                padrao = 0;
                break;
            case MODO_FESTA:
                padrao = 1;
                break;
            case MODO_SEGURANCA:
                padrao = 2;
                break;
            default:
                padrao = 3; // Modo padrão desligado
                break;
        }
        
        exibir_padrao(padrao);
    }
    else {
        clear_matrix(pio, sm);
        update_leds(pio, sm);
    }
}

void piscar_matriz_intruso() {
    static uint32_t ultima_execucao = 0;
    static bool ligada = false;
    uint32_t agora = to_ms_since_boot(get_absolute_time());

    if (agora - ultima_execucao > 500) {
        if (ligada) {
            clear_matrix(pio0, 0);
        } else {
            // Exibir padrão de alerta (vermelho intenso)
            for (int i = 0; i < NUM_LEDS; i++) {
                leds[i] = create_color(0, 255, 0); // Vermelho intenso
            }
        }
        update_leds(pio0, 0);
        ligada = !ligada;
        ultima_execucao = agora;
    }
}

