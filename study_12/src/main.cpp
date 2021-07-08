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

SemaphoreHandle_t mutex_smp;
TaskHandle_t Task1Handler, Task2Handler;

void vTask1(void *pvParameters);
void vTask2(void *pvParameters);
void vSendInfo(int c){
  xSemaphoreTake(mutex_smp, portMAX_DELAY);
  Serial.println("Sending info from Task: " + String(c));
  delay(1000);
  xSemaphoreGive(mutex_smp);
}


void setup() {
  Serial.begin(9600);
  pinMode(LED, OUTPUT);

  mutex_smp = xSemaphoreCreateMutex();
  xTaskCreate(vTask1, "vTask1", configMINIMAL_STACK_SIZE+1024, NULL, 1, &Task1Handler);
  xTaskCreate(vTask2, "vTask2", configMINIMAL_STACK_SIZE+1024, NULL, 3, &Task2Handler);
}

void loop() {
  digitalWrite(LED, HIGH);
  vTaskDelay(200);
  digitalWrite(LED, LOW);
  vTaskDelay(800);
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
    /*needed to free CPU use and allow the other task to get it*/
    vTaskDelay(10);
  }
}
