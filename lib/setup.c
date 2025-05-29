#include "setup.h"
#include "inc/ssd1306.h"

// Inicializa os GPIOs conectados aos LEDs
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
    //Inicializar display
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