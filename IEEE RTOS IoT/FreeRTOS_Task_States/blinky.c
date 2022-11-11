/*
 * blinky.c
 *
 *  Created on: 31 oct 2022
 *      Author: danie
 */

/* Standard includes. */
#include <stdbool.h>
#include <stdint.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"

/* Hardware includes */
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"

extern TaskHandle_t third_blinky_Handle;

void main_blinky( void* pvArgs ){
    uint16_t LEDVal = 0;
    int i;
    for(i=0; i<10; i++){
        LEDVal ^= GPIO_PIN_0;
        GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, LEDVal);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
    vTaskResume(third_blinky_Handle);
    vTaskDelete(NULL);
}

void secondary_blinky( void* pvArgs ){
    uint16_t LEDVal = 0;
    while(1){
        LEDVal ^= GPIO_PIN_1;
        GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, LEDVal);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
    vTaskDelete(NULL);
}

void third_blinky( void* pvArgs ){
    uint16_t LEDVal = 0;
    LEDVal ^= GPIO_PIN_0;
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0, LEDVal);
    vTaskDelay(pdMS_TO_TICKS(1000));
    vTaskSuspend(NULL);
    while(1){
        LEDVal ^= GPIO_PIN_0;
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0, LEDVal);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
    vTaskDelete(NULL);
}


