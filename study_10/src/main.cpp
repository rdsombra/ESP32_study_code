/*This code intends to show how to work with binary semaphores
*on interruptions - ISR
*by Rafael Sombra.
*/
#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"

#define LED 2
#define BT 12

SemaphoreHandle_t xBinSemaphore;
TaskHandle_t xTaskTreatButtonHandler;

void vTaskTreatButton(void *pvParameters);

/*Callback function to deal with button event.
* it will yield the execution to the higher priority
* task when the button is pressed */
void ISR_CallBack(){
  BaseType_t xHighPriorityTaskWoken = pdTRUE;
  xSemaphoreGiveFromISR(xBinSemaphore, &xHighPriorityTaskWoken);
  if (pdTRUE == xHighPriorityTaskWoken){
    portYIELD_FROM_ISR();
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(LED, OUTPUT);
  pinMode(BT, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BT), ISR_CallBack, FALLING);

  xBinSemaphore = xSemaphoreCreateBinary();
  xTaskCreate(vTaskTreatButton, "Task BT", configMINIMAL_STACK_SIZE + 1024,
              NULL, 3, &xTaskTreatButtonHandler);
  
}

void loop() {
  digitalWrite(LED, HIGH);
  vTaskDelay(pdMS_TO_TICKS(100));
  digitalWrite(LED, LOW);
  vTaskDelay(pdMS_TO_TICKS(900));
}


/*This task will be called after the callback function.
* It will wait for the semaphore and print the counter value*/
void vTaskTreatButton(void *pvParameters){
  int counter = 0;
  while(1){
    xSemaphoreTake(xBinSemaphore, portMAX_DELAY);
    Serial.println("conter: "+ String(counter++));

  }
}