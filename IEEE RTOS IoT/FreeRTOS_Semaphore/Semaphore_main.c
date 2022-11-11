
#include <stdbool.h>

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "utils/uartstdio.h"
#include "driverlib/uart.h"
#include "inc/hw_memmap.h"

extern SemaphoreHandle_t Sempahore1;

void UART_ISR_Handler (void){
    BaseType_t priorityTaskWoken = pdFALSE;
    xSemaphoreGiveFromISR(Sempahore1, &priorityTaskWoken);
    portYIELD_FROM_ISR(priorityTaskWoken);
    UARTIntClear(UART0_BASE, UART_INT_RX);
}

//Pedir la creacion de una tartea que lea el dato recibido en una cola
void Task1 (void* args){
    unsigned char RxBuff;
    while(1){
        UARTprintf("\nIntroduzca el numero que quiera: ");
        xSemaphoreTake(Sempahore1, portMAX_DELAY);
        RxBuff = UARTgetc();
        UARTprintf("\nTarea 1 ha recibido el caracter \"%c\"", RxBuff);
    }
    vTaskDelete(NULL);
}

// Tarea 2 recibe dato de ISR y lo madna a cola
