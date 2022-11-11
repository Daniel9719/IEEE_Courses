
#include <stdbool.h>

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "utils/uartstdio.h"
#include "driverlib/uart.h"
#include "inc/hw_memmap.h"

extern SemaphoreHandle_t Semaphore1;
extern SemaphoreHandle_t Semaphore2;

void UART_ISR_Handler (void){
    BaseType_t priorityTaskWoken = pdFALSE;
    xSemaphoreGiveFromISR(Semaphore1, &priorityTaskWoken);
    portYIELD_FROM_ISR(priorityTaskWoken);
    UARTIntClear(UART0_BASE, UART_INT_RX);
}

//Pedir la creacion de una tartea que lea el dato recibido en una cola
void Task1 (void* args){
    unsigned char RxBuff;
    while(1){
        xSemaphoreTake(Semaphore2, portMAX_DELAY);
        UARTprintf("Introduzca el numero que quiera:\n");
        xSemaphoreTake(Semaphore1, portMAX_DELAY);
        RxBuff = UARTgetc();
        UARTprintf("Tarea 1 ha recibido el caracter \"%c\"\n", RxBuff);
        xSemaphoreGive(Semaphore2);
    }
    vTaskDelete(NULL);
}

void Task2 (void* args){
    uint32_t Contador = 0;
    uint32_t i = 0;
    while(1){
        xSemaphoreTake(Semaphore2, portMAX_DELAY);
        UARTprintf("Tarea 2 corriendo\n");
        for(i=0; i<5000000; i++){
            // Nada
            Contador++;
        }
        xSemaphoreGive(Semaphore2);
        UARTprintf("Tarea 2 finalizada\n");
    }
    vTaskDelete(NULL);
}

void Task3 (void* args){
    uint32_t Contador = 0;
    uint32_t i = 0;
    while(1){
        xSemaphoreTake(Semaphore2, portMAX_DELAY);
        UARTprintf("Tarea 3 corriendo\n");
        for(i=0; i<5000000; i++){
            // Nada
            Contador++;
        }
        xSemaphoreGive(Semaphore2);
        UARTprintf("Tarea 3 finalizada\n");
    }
    vTaskDelete(NULL);
}
