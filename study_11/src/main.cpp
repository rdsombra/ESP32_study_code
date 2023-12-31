/*This code intends to show how to work with count semaphores
* on interruptions - ISR
* The counting semaphore works like a box were all the ISR are
* deposited and will be removed/treated one by one. It causes the
* ISR count to decrease.
*by Rafael Sombra.
*/
#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"

#define LED 2
#define BUTTON 12

SemaphoreHandle_t xCountSemaphore;
TaskHandle_t xTaskTreatButtonHandler;

void vTaskTreatButton(void *pvParameters);

/*Callback function to deal with button event.
* It will yield the execution to the higher priority
* task when the button is pressed. */
void ISR_CallBack(){
  BaseType_t xHighPriorityTaskWoken = pdTRUE;
  xSemaphoreGiveFromISR(xCountSemaphore, &xHighPriorityTaskWoken);
  if (pdTRUE == xHighPriorityTaskWoken){
    portYIELD_FROM_ISR();
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(LED, OUTPUT);
  pinMode(BUTTON, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BUTTON), ISR_CallBack, FALLING);

  xCountSemaphore = xSemaphoreCreateCounting(255, 0);
  xTaskCreate(vTaskTreatButton, "Task BT", configMINIMAL_STACK_SIZE + 1024,
              NULL, 3, &xTaskTreatButtonHandler);
}

void loop() {
  digitalWrite(LED, !digitalRead(LED));
  vTaskDelay(pdMS_TO_TICKS(500));
}

/*This task will be called after the callback function.
* It will wait for the semaphore and print the counter value*/
void vTaskTreatButton(void *pvParameters){
  BaseType_t x = 0;
  while(1){
    xSemaphoreTake(xCountSemaphore, portMAX_DELAY);
    Serial.println("Treating a ISR from button.");
    Serial.print("IRS counter: ");
    x = uxSemaphoreGetCount(xCountSemaphore);
    Serial.println(x);
    delay(1000);
  }
}