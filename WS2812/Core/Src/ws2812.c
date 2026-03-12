#include "ws2812.h"

#define Code0 30
#define Code1 60
#define Codereset 0

void WS2812_Update(void){
    static uint16_t data[] = {
        Code1, Code1, Code1, Code1, Code1, Code1, Code1, Code1,
        Code0, Code0, Code0, Code0, Code0, Code0, Code0, Code0,
        Code0, Code0, Code0, Code0, Code0, Code0, Code0, Code0,
        Codereset
    };

    HAL_TIM_PWM_Start_DMA(&htim3, TIM_CHANNEL_1, (uint32_t*)data, sizeof(data) / sizeof(uint16_t));
}


