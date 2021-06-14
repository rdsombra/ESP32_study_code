/*
* Study of how to pass parameters at task creation
* by Rafael Sombra
*/
#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/*pin mapping*/ 
#define LED_A 2
#define LED_B 14

/* aux variables */
int timer1 = 10;

TaskHandle_t task1Handle = NULL;
TaskHandle_t task2Handle = NULL;
TaskHandle_t task3Handle = NULL;

/*Task prototypes*/
void vTaskBlink(void *pvParameters);
void vTask2(void *pvParameters);

void setup() {
  Serial.begin(9600);
  /*Here, instead of just null, we passa a void pointer to the LED_A definition
   *as parameter  */
  xTaskCreate(vTaskBlink,"TASK1",configMINIMAL_STACK_SIZE,(void*)LED_A,1,&task1Handle);
  /*passing timer1 as parameter for task2*/
  xTaskCreate(vTask2,"TASK2",configMINIMAL_STACK_SIZE+1024,(void*)timer1,2,&task2Handle);
  xTaskCreate(vTaskBlink,"TASK3",configMINIMAL_STACK_SIZE,(void*)LED_B,1,&task3Handle);
}

void loop() {
  /*frees CPU for 3 sec*/
  vTaskDelay(3000); 
}

/* vTask1: inverts the LED state*/
void vTaskBlink(void *pvParameters) {
    int pin = (int)pvParameters;
    pinMode(pin,OUTPUT);
    while (1){
      digitalWrite(pin,!digitalRead(pin));
      vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

/* vTask2: prints count value each second*/
void vTask2(void *pvParameters){
  /* receiving the timer parameter */
  int my_timer = (int)pvParameters;
  int count = 0;
  while (1){
    Serial.println("Task2: " + String(count++));
    if(count == my_timer){
      Serial.println("suspending task1...");
      vTaskSuspend(task1Handle);
      digitalWrite(LED_A, LOW);
    }
    if(count == my_timer+5){
      Serial.println("calling back task 1...");
      vTaskResume(task1Handle);
      count = 0;
      Serial.println("back to beginning");
    }
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}