#include <stdio.h>
#include <stdint.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"

//ADC mux0: pin 26
//ADC mux1: pin 27
//ADC mux2: pin 28

int main(){
    stdio_init_all();
    adc_init(); 
    //We will use the GPIO 26
    adc_gpio_init(26);

    adc_select_input(0);    //here we select one channel of the ADC (there is a mux there)
    //conversion of the values
    const float conversion_factor = 3.3f/(4096);
    while(1){
        uint16_t result = adc_read();
        printf("Raw value: 0x%03x, voltage: %f \n", result, result*conversion_factor);


    }
}

