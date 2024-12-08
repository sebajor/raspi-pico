#include "pico/stdlib.h"

const int led_sleep = 250;


int main(){
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
    while(1){
        gpio_put(PICO_DEFAULT_LED_PIN, 0);
        sleep_ms(led_sleep);
        gpio_put(PICO_DEFAULT_LED_PIN, 1);
        sleep_ms(led_sleep);
    }
    return 1;
}

