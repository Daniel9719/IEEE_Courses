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
// #include "inc/hw_memmap.h"
// #include "driverlib/gpio.h"
// #include "utils/uartstdio.h"

void Task1( void* pvArgs ){
    for( ; ; ){
    }
    vTaskDelete(NULL);
}

void Task2( void* pvArgs ){
    while(1){
        // GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, LEDVal);
        // UARTprintf("Corriendo Tarea 1...\n");
    }
    vTaskDelete(NULL);
}



