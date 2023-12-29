#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freeRTOS/task.h"
#include "freertos/queue.h"

#define LED 2

QueueHandle_t xQ;
TaskHandle_t xTask1Handler;
TaskHandle_t xTask2Handler;

void vTask1(void *pvParameters);
void vTask2(void *pvParameters);


void setup() {
  Serial.begin(9600);
  pinMode(LED, OUTPUT);
  BaseType_t xReturned;
  xQ = xQueueCreate(5, sizeof(int));

  if (NULL == xQ){
    Serial.println("It was not possible to create the queue");
    /*blocking program exec*/
    while(1);
  }

  xReturned = xTaskCreate(
    vTask1, "Task1", configMINIMAL_STACK_SIZE+1024, NULL, 1, &xTask1Handler);
  if(pdFAIL == xReturned){
    Serial.println("Failed to create task1.");
    /*blocking program exec*/
    while (1);    
  }
  xReturned = xTaskCreate(
    vTask2, "Task2", configMINIMAL_STACK_SIZE+1024, NULL, 1, &xTask2Handler);
  if(pdFAIL == xReturned){
    Serial.println("Failed to create task2.");
    /*blocking program exec*/
    while (1);    
  }
}

void loop() {
  digitalWrite(LED, !digitalRead(LED));
  vTaskDelay(pdMS_TO_TICKS(1000));
}

/* adds a value to the queue each 500ms. 
* Resets the counter after 10 rounds and wait 5 sec. */
void vTask1(void *pvParameters){
  int count = 0;
  while(1){
    if(count < 10){
      xQueueSend(xQ, &count, portMAX_DELAY);
      count++;
    }
    else{
      count = 0;
      /* triggers a timeout on task2. */
      vTaskDelay(pdMS_TO_TICKS(5000));
    }
    vTaskDelay(pdMS_TO_TICKS(500));
  }
}

/* Reads the values from the queue every 1 second. */
void vTask2(void *pvParameters){
  int received_value = 0;
  while(1){
    if(pdTRUE == xQueueReceive(xQ, &received_value, pdMS_TO_TICKS(1000))){
      Serial.println("Received value: " + String(received_value));
    }
    else Serial.println("TIMEOUT!!!");
  }
}