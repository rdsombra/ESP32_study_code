#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/*pin mapping*/ 
#define LED 2

TaskHandle_t task1Handle = NULL;
TaskHandle_t task2Handle = NULL;

/*Task prototypes*/
void vTask1(void *pvParameters);
void vTask2(void *pvParameters);

void setup() {
  Serial.begin(9600);

  /*creating 2 tasks and setting their priority*/
  xTaskCreate(vTask1,"TASK1",configMINIMAL_STACK_SIZE,NULL,1,&task1Handle);
  xTaskCreate(vTask2,"TASK2",configMINIMAL_STACK_SIZE+1024,NULL,2,&task2Handle);
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
      vTaskDelay(pdMS_TO_TICKS(500));
    }
}

/* vTask2: prints count value each second*/
void vTask2(void *pvParameters){
  int cont = 0;
  while (1){
    Serial.println("Task 2: " + String(cont++));
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}