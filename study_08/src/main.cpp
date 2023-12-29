/*** 
* Demonstrating how to add values to a queue from a ISR. 
* By Rafael Sombra.
***/

#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

/* pin definition according w/ the board used: ESP32 Doit Devkit v1. */
#define LED 2
#define BUTTON 12

QueueHandle_t xQ;
TaskHandle_t xTask1Handler;

void vTask1(void *pvParameters);

void TreatISR_BT(){
  static int value;
  value++;
  xQueueSendFromISR(xQ, &value, NULL);
}

void setup() {
  Serial.begin(9600);
  pinMode(LED, OUTPUT);
  pinMode(BUTTON, INPUT_PULLUP);

  /* Creating ISR from the button press, 
  * triggered by the falling edge of the signal. */
  attachInterrupt(digitalPinToInterrupt(BUTTON), TreatISR_BT, FALLING);

  xQ = xQueueCreate(5, sizeof(int));
    if (NULL == xQ){
    Serial.println("It was not possible to create the queue");
    /*blocking program exec*/
    while(1);
  }

  xTaskCreate(vTask1, "Task 1", configMINIMAL_STACK_SIZE+1024,NULL,1,&xTask1Handler);
}

void loop() {
  digitalWrite(LED, !digitalRead(LED));
  vTaskDelay(pdMS_TO_TICKS(1000));
}

void vTask1(void *pvParameters){
  int received_val;
  while(1){
    xQueueReceive(xQ, &received_val, portMAX_DELAY);
    Serial.println("Button was pressed!!! "+ String(received_val));
  }
}