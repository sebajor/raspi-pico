#include <stdio.h>
#include <stdint.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/dma.h"


constexpr int samples = 128;
const int adc_pin = 26;
const int adc_channel = 0;
const int Fs = 10*1e6;  

uint8_t capture_buffer[samples];

int main(){
     stdio_init_all();

     adc_gpio_init(adc_pin);
     adc_init();
     adc_select_input(adc_channel);

     //setup a FIFO --> the fifos are of 8bits
     adc_fifo_setup(
        1,          //write each completed conversion ot the fifo
        1,          //enable DMA data request (DREQ)
        1,          //DREQ and IRQ asserted when there is at least 1 sample available
        0,          //you can use the last bit of the ADC to check correctness, we dont care and leave that option off
        1           //shift each sample when pushing to FIFO
    );
     //set sampling freq.. by default it takes 96 cycles to get one sample so the 
     //the adc samples at 48_000_000
    adc_set_clkdiv(0);     //we are sampling at full speed, 

    //setup the DMA
    uint dma_channel = dma_claim_unused_channel(1);
    dma_channel_config cfg = dma_channel_get_default_config(dma_channel);
    channel_config_set_transfer_data_size(&cfg, DMA_SIZE_8);    //the output of the fifo is 8bis
    channel_config_set_read_increment(&cfg, 0);                //always read the same location
    channel_config_set_write_increment(&cfg, 1);

    //set the DMA data request to the adc flag
    channel_config_set_dreq(&cfg, DREQ_ADC);

    dma_channel_configure(dma_channel, &cfg,
            capture_buffer,      //destination
            &adc_hw->fifo,      //source
            samples,            //transfer count
            1                   //start inmediatly
            );
    //start the adc campture
    adc_run(1);


    //We wait until the DMA finish...
    dma_channel_wait_for_finish_blocking(dma_channel);
    //clean the adc
    adc_run(0);
    adc_fifo_drain();
    
    //print the data
    while(1){
        printf("This are the collected samples\n");
        for(int i=0; i<samples; ++i){
            printf("%-3d\n", capture_buffer[i]);
        }
        printf("...\n");
    }
    return 1;
}

