/*This is a study code to work with mutex semaphores on FreeRTOS.
* I also try to show how to deal with resource access control and 
* priority inversion.
* by Rafael Sombra.
*/

#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"

#define LED 2

SemaphoreHandle_t xSemphrMutexHandle;
TaskHandle_t xTask1Handler, xTask2Handler;

void vTask1(void *pvParameters);
void vTask2(void *pvParameters);
void vSendInfo(int c);

void setup() {
  Serial.begin(9600);
  pinMode(LED, OUTPUT);

  xSemphrMutexHandle = xSemaphoreCreateMutex();
  xTaskCreate(vTask1, "vTask1", configMINIMAL_STACK_SIZE+1024, NULL, 1, &xTask1Handler);
  xTaskCreate(vTask2, "vTask2", configMINIMAL_STACK_SIZE+1024, NULL, 3, &xTask2Handler);
}

void loop() {
  digitalWrite(LED, !digitalRead(LED));
  vTaskDelay(500);
}

void vTask1(void *pvParameters){
  while(1){
    vSendInfo(1);
    vTaskDelay(10);
  }
}

void vTask2(void *pvParameters){
  while(1){
    vSendInfo(2);
    vTaskDelay(10);
  }
}

void vSendInfo(int c){
  xSemaphoreTake(xSemphrMutexHandle, portMAX_DELAY);
  Serial.println("Sending info from Task: " + String(c));
  delay(1000);
  xSemaphoreGive(xSemphrMutexHandle);
}