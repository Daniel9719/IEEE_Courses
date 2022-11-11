
#include "FreeRTOS.h"
#include "task.h"

#include "utils/uartstdio.h"

extern volatile uint8_t num;

void Task1 (void* args){
    int i;
    for(i=0; i<5; i++){
        UARTprintf("Corriendo Tarea 1...\n");
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
    vTaskDelete(NULL);
}

void Task2 (void* args){
    int i;
    for(i=0; i<5; i++){
        UARTprintf("Corriendo Tarea 2...\n");
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
    vTaskDelete(NULL);
}

void Task3 (void* args){
    int i;
    for(i=0; i<5; i++){
        if(i==2){
            vTaskPrioritySet(NULL, 4);
        }
        UARTprintf("Corriendo Tarea 3 y valor %d...\n", num);
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
    vTaskDelete(NULL);
}
