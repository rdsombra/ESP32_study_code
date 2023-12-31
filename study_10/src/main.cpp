/*This code intends to show how to work with binary semaphores
* on interruptions - ISR. Everytime an ISR occurs, a higher priority task
* is called and the counter of triggers (from button) is printed on the 
* serial output.
*by Rafael Sombra.
*/
#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"

#define LED 2
#define BUTTON 12

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
  pinMode(BUTTON, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BUTTON), ISR_CallBack, FALLING);

  xBinSemaphore = xSemaphoreCreateBinary();
  /*Creating task w/ higher priority.*/
  xTaskCreate(vTaskTreatButton, "Task Button", configMINIMAL_STACK_SIZE + 1024,
              NULL, 3, &xTaskTreatButtonHandler);
  
}

void loop() {
  /* Using the loop only to invert the LED state.*/
  digitalWrite(LED, !digitalRead(LED));
  vTaskDelay(pdMS_TO_TICKS(1000));
}


/*This task will be called after the callback function.
* It will wait for the semaphore and print the counter value*/
void vTaskTreatButton(void *pvParameters){
  int counter = 0;
  while(1){
    xSemaphoreTake(xBinSemaphore, portMAX_DELAY);
    Serial.println("counter: " +String(counter++) +"\n");
  }
}