/*
* Study of task deletion
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
    Serial.println("Task 2: " + String(count++));
    if(count == 10){
      if (NULL != xTask1Handle){
        Serial.println("Deleting task1");
        vTaskDelete(xTask1Handle);
        digitalWrite(LED, LOW);
        xTask1Handle = NULL;
      }
    }
    if(count == 15){
      vTaskDelete(xTask2Handle);
    }
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}