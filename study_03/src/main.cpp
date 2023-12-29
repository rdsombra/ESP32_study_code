/*
* Study of task suspention and resume
* by Rafael Sombra
*/
#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/*pin mapping*/ 
#define LED 2

TaskHandle_t xTask1Handle = NULL;
TaskHandle_t xTask2Handle = NULL;

/*Task prototypes*/
void vTask1(void *pvParameters);
void vTask2(void *pvParameters);

void setup() {
  Serial.begin(9600);
  xTaskCreate(vTask1,"TASK1",configMINIMAL_STACK_SIZE,NULL,1,&xTask1Handle);
  xTaskCreate(vTask2,"TASK2",configMINIMAL_STACK_SIZE+1024,NULL,2,&xTask2Handle);
}

void loop() {
  /*frees CPU for 3 sec*/
  vTaskDelay(3000); 
}

/* vTask1: inverts the LED state*/
void vTask1(void *pvParameters) {
    pinMode(LED,OUTPUT);
    while (1){
      digitalWrite(LED,!digitalRead(LED));
      Serial.println("---");
      vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

/* vTask2: prints count value each second*/
void vTask2(void *pvParameters){
  int count = 0;
  while (1){
    Serial.println("Task2: " + String(count++));
    /* suspend task1 when count==10 */
    if(count == 10){
      Serial.println("suspending task1...");
      vTaskSuspend(xTask1Handle);
      digitalWrite(LED, LOW);
    }
    /* resume task1 when count==15 */
    if(count == 15){
      Serial.println("calling back task 1...");
      vTaskResume(xTask1Handle);
      count = 0;
      Serial.println("back to beginning");
    }
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}