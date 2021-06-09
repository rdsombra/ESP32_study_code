/*************************************************************
 * This is an study code to create tasks.
 * Here I will show to create tasks on FreeRTOS
 * by: Rafael Sombra
 * ***********************************************************/

#include <Arduino.h>
/*to access FreeRTOS's interfaces*/
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/*pin map for ESP32 DOIT devkit*/
#define LED 2

/*variables to hold task handles */
TaskHandle_t task1Handle = NULL;
TaskHandle_t task2Handle = NULL;

/*task prototypes */
void vTask1(void *pvParameters);
void vTask2(void *pvParameters);

void setup() { 
  Serial.begin(9600);
  pinMode(LED, OUTPUT);

  xTaskCreate(vTask1, "TASK1", configMINIMAL_STACK_SIZE, NULL, 1, &task1Handle);
  xTaskCreate(vTask1, "TASK1", configMINIMAL_STACK_SIZE+1024, NULL, 2, &task1Handle);
}

void loop() {
  vTaskDelay(3000);
}

void vTask1(void *pvParameters){
  for (;;){
    digitalWrite(LED,!digitalRead(LED));
    vTaskDelay(pdMS_TO_TICKS(200));
  }
};

void vTask2(void *pvParameters){
  int count=0;
  while(1){
    /*print on serial port*/
    Serial.println("Task2: " + String(count++));
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
};
