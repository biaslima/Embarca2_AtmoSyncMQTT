#ifndef MODOS_H
#define MODOS_H

// Tipos de modo do sistema
typedef enum {
    MODO_HOME,
    MODO_FESTA,
    MODO_SEGURANCA,
} ModoSistema;

extern ModoSistema modo_atual;

// Protótipos das funções
void set_modo(ModoSistema novo_modo);
void atualiza_display(void);
void atualiza_matriz_leds(void);
void atualiza_buzzer(void);
void atualiza_rgb_led(void);
void executar_modulo_modos(void);
bool detect_loud_noise(void);
void atualizar_tempo_simulado();


#endif