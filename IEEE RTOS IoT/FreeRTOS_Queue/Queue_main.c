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
    }; 
    int i = 0;
    for(i=0; i<5; i++){
        // Preguntar que va a suceder con prioridades iguales y diferentes
        // UARTprintf("Tarea 1 manda valor %d\n", num);
        UARTprintf("Tarea 1 manda valor %d de ID %s\n", PPG.Data_Val, PPG.ID_Val);
        // xQueueSend(Queue1, &num, portMAX_DELAY);
        xQueueSend(Queue1, &PPG, portMAX_DELAY);
        // num++;
        PPG.Data_Val++;
    }
    vTaskDelete(NULL);
}

void Task2 (void* args){
    uint8_t num = 0;
    struct Data_t RxData;
    int i = 0;
    for(i=0; i<5; i++){
        // xQueueReceive(Queue1, &num, portMAX_DELAY);
        xQueueReceive(Queue1, &RxData, portMAX_DELAY);
        // UARTprintf("Tarea 2 recibe valor %d\n", num);
        UARTprintf("Tarea 2 recibe valor %d de ID %s\n", RxData.Data_Val, RxData.ID_Val);
        num++;
        xQueueOverwrite(Queue2, &RxData.Data_Val);
        UARTprintf("Tarea 2 manda dato %d a buzon\n", num);
    }
    vTaskDelete(NULL);
}

void Task3 (void* args){
    uint8_t num = 44;
    struct Data_t EDA = {
        .ID_Val = "EDA",
        .Data_Val = 3
    };
    
    while(1){
        // Evidenciar uso de estructuras como miembros al no usarlas primero
        // UARTprintf("Tarea 3 manda valor %d al frente de la cola\n", num);
        UARTprintf("Tarea 3 manda valor %d al frente de la cola desde ID %s\n",
                    EDA.Data_Val, EDA.ID_Val);
        // xQueueSendToFront(Queue1, &num, portMAX_DELAY);
        xQueueSendToFront(Queue1, &EDA, portMAX_DELAY);
        vTaskDelay(pdMS_TO_TICKS(100));
        xQueuePeek(Queue2, &num, portMAX_DELAY);
        UARTprintf("Tarea 3 recibe valor %d del buzon\n", num);
    }
    vTaskDelete(NULL);
}
