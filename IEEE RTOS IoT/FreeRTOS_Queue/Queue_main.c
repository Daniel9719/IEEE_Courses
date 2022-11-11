#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "utils/uartstdio.h"

#include "Queue_main.h"

extern QueueHandle_t Queue1;
extern QueueHandle_t Queue2;

void Task1 (void* args){
    uint8_t num = 0;
    struct Data_t PPG = {
        .ID_Val = "PPG",
        .Data_Val = 32
        // .Data_Val = LOW
    }; 
    int i = 0;
    for(i=0; i<5; i++){
        xQueueSend(Queue1, &num, portMAX_DELAY);
        UARTprintf("Tarea 1 manda valor %d\n", num);
        num++;
        // UARTprintf("Tarea 1 manda valor %d de ID %s\n", PPG.Data_Val, PPG.ID_Val);
        // xQueueSend(Queue1, &PPG, portMAX_DELAY);
        // PPG.Data_Val++;
    }
    vTaskDelete(NULL);
}

void Task2 (void* args){
    uint8_t num = 0;
    struct Data_t RxData;
    while(1){
        xQueueReceive(Queue1, &num, portMAX_DELAY);
        UARTprintf("Tarea 2 recibe valor %d\n", num);
        // xQueueReceive(Queue1, &RxData, portMAX_DELAY);
        // UARTprintf("Tarea 2 recibe valor %d de ID %s\n", RxData.Data_Val, RxData.ID_Val);

        // xQueueOverwrite(Queue2, &RxData.Data_Val);
        // UARTprintf("Tarea 2 manda dato %d a buzon\n", num);
        // num++;
    }
    vTaskDelete(NULL);
}

void Task3 (void* args){
    int i = 0;
    uint8_t num = 44;
    struct Data_t EDA = {
        .ID_Val = "EDA",
        .Data_Val = 3
        // .Data_Val = HIGH
    };
    
    for(i=0; i<5; i++){
        xQueueSendToFront(Queue1, &num, portMAX_DELAY);
        UARTprintf("Tarea 3 manda valor %d al frente de la cola\n", num);
        // UARTprintf("Tarea 3 manda valor %d al frente de la cola desde ID %s\n",
        //             EDA.Data_Val, EDA.ID_Val);
        // xQueueSendToFront(Queue1, &EDA, portMAX_DELAY);
        // vTaskDelay(pdMS_TO_TICKS(100));

        // xQueuePeek(Queue2, &num, portMAX_DELAY);
        // UARTprintf("Tarea 3 recibe valor %d del buzon\n", num);
    }
    vTaskDelete(NULL);
}
