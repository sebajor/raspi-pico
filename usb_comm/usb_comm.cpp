#include "pico/stdlib.h"
#include <stdio.h>
#include <string>



int main(){
    stdio_init_all();
    char word[256];
    while(1){
        scanf("%s", word);
        sleep_ms(10);
        printf("%s", word);
    }
}
