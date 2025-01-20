#include <stdio.h>
#include <stdint.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"



const float ADCCLK = 48000000;
const float ADC_FS = 1000;   

const float conversion_factor = 3.3f/(4096);    //factor to convert to voltage

int main(){
    stdio_init_all();
    sleep_ms(50);
    for(int i=0; i<3; ++i){
        adc_gpio_init(26+i);
    }
    adc_select_input(0);
    adc_init();
    adc_set_round_robin(7);

    adc_set_clkdiv(static_cast<float>(ADCCLK/ADC_FS*3));
    sleep_ms(3000);

    char msg {};
    uint16_t value = 0;
    while(1){
        msg = getchar();
        value = adc_read();
        printf("Raw value: 0x%03x, voltage: %f \n", result, result*conversion_factor);

    }
}
