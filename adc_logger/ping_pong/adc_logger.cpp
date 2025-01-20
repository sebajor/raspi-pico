#include <stdio.h>
#include <stdint.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/dma.h"

/*
 *  The idea is to follow the template in https://github.com/vha3/Hunter-Adams-RP2040-Demos/blob/master/Audio/f_Audio_FFT/fft.c
 *  where he use two DMA channels, one capture data continously and the other one is
 *  in charge of reset the read address of the data capture DMA meanwhile the processor
 *  do other stuffs.. 
 *  The only difference is that we make all the stuffs in the main function bcs why not
 *
 */

constexpr int samples = 1024;//128;
constexpr int n_adcs = 4;
const float ADCCLK = 48000000;
const float ADC_FS = 2000;  //with 1000 I got 750Hz of fs
//const float ADC_FS = 3000; 


const float conversion_factor = 3.3f/(256);    //factor to convert to voltage

uint8_t capture_buffer0[n_adcs*samples];
uint8_t capture_buffer1[n_adcs*samples];
uint8_t* capture_buffer0_pointer = &capture_buffer0[0];
uint8_t* capture_buffer1_pointer = &capture_buffer1[0];

uint8_t adc0_data[samples];
uint8_t adc1_data[samples];
uint8_t adc2_data[samples];

void adc_round_robin_config(){
    for(int i=0; i<n_adcs; ++i){
        adc_gpio_init(26+i);
    }
    adc_select_input(0);
    adc_init();

    //set the bitmask
    adc_set_round_robin((1<<(n_adcs))-1);
    //adc_set_clkdiv(7);

    adc_fifo_setup(
        1,          //write each completed conversion ot the fifo
        1,          //enable DMA data request (DREQ)
        1,          //DREQ and IRQ asserted when there is at least 1 sample available
        0,          //you can use the last bit of the ADC to check correctness, we dont care and leave that option off
        1           //shift each sample when pushing to FIFO (to have 8 bits samples)
    );
    //adc_set_clkdiv(static_cast<float>(ADCCLK/ADC_FS*4));  //I got around 180Hz of sampling rate..
    adc_set_clkdiv(static_cast<float>(ADCCLK/ADC_FS/n_adcs)); 
    //adc_set_clkdiv(8192.f);
    //
}


int main(){
    stdio_init_all();

    //ad
    //char msg[256];
    char msg {};
    adc_round_robin_config();
    sleep_ms(3000);

    //set data dma, read from cte address writting in increasing way
    int data_dma0 = dma_claim_unused_channel(1);
    dma_channel_config conf_data_dma0 = dma_channel_get_default_config(data_dma0);
    channel_config_set_transfer_data_size(&conf_data_dma0, DMA_SIZE_8);
    channel_config_set_read_increment(&conf_data_dma0, 0);
    channel_config_set_write_increment(&conf_data_dma0, 1); 
    //set the data request based on the availability of ADC samples
    channel_config_set_dreq(&conf_data_dma0, DREQ_ADC);
    
    dma_channel_configure(
        data_dma0,
        &conf_data_dma0,     //configuration struct
        capture_buffer0,     //dest
        &adc_hw->fifo,      //src
        4*samples,          //number of samples
        0                   //dont start right away
    );


    int data_dma1 = dma_claim_unused_channel(1);
    dma_channel_config conf_data_dma1 = dma_channel_get_default_config(data_dma1);
    channel_config_set_transfer_data_size(&conf_data_dma1, DMA_SIZE_8);
    channel_config_set_read_increment(&conf_data_dma1, 0);
    channel_config_set_write_increment(&conf_data_dma1, 1); 
    //set the data request based on the availability of ADC samples
    channel_config_set_dreq(&conf_data_dma1, DREQ_ADC);
    
    dma_channel_configure(
        data_dma1,
        &conf_data_dma1,     //configuration struct
        capture_buffer1,     //dest
        &adc_hw->fifo,      //src
        4*samples,          //number of samples
        0                   //dont start right away
    );


    //here I will wait until got a signal from the usb connection
    msg =getchar(); //this is blocking right?
    

    //I dont fully get why we need a dma to reset the address and you cant do it 
    //irght away just writting the address...
    //start adc
    dma_channel_start(data_dma0);
    adc_run(1);
    while(1){
        dma_channel_wait_for_finish_blocking(data_dma0);
        adc_fifo_drain(); //CHECK!
        adc_select_input(0);  //CHECK!!
        dma_channel_start(data_dma1);
        //print while the other dma channel is taking data
        printf("%c%c%c%c", 0xaa,0xbb, 0xcc, 0xdd);
        fwrite(capture_buffer0, 1, n_adcs*samples, stdout);

        dma_hw->ch[data_dma0].write_addr = (long int)capture_buffer0_pointer;  //chcec!!
         
        dma_channel_wait_for_finish_blocking(data_dma1);
        adc_fifo_drain(); //CHECK!
        adc_select_input(0);  //CHECK!!
        dma_channel_start(data_dma0);
        //print while the other dma channel is taking data
        printf("%c%c%c%c", 0xaa,0xbb, 0xcc, 0xdd);
        fwrite(capture_buffer1, 1, n_adcs*samples, stdout);
        dma_hw->ch[data_dma1].write_addr = (long int)capture_buffer1_pointer;  //chcec!!

        //dma_channel_start(control_dma);
        //now do somehting...
        //In theory if you would like to have data running all the time you should
        //put the channel_start after the wait_for_finish since then you are processing
        //the data while its being written and dont lose computing cycles
    }
}

