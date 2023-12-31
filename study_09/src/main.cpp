/*
*This code is a study of binary semaphores on FreeRTOS.
*I'm using a binary semaphore to trigger an analog read of a pontenciometer
*plugged on pin 34 everytime the loop taks inverts LED status and
*frees the semaphore.
*by: Rafael Sombra.
*/
#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"

#define LED 2
#define ANALOG_INPUT 34

TaskHandle_t xTaskADCHandle;
SemaphoreHandle_t xBinSemaphore;

void vTaskADC(void *pvParameters);

void setup() {
  Serial.begin(9600);
  pinMode(LED, OUTPUT);

  xBinSemaphore = xSemaphoreCreateBinary();
  if (NULL == xBinSemaphore){
    Serial.println("Failed to create semaphore!!!");
    while(1);
  }

  xTaskCreate(vTaskADC, "TaskADC", configMINIMAL_STACK_SIZE + 1024,
              NULL, 1, &xTaskADCHandle);
}

void loop() {
  digitalWrite(LED, !digitalRead(LED));
  Serial.println("LED current status: " + String(digitalRead(LED)));
  vTaskDelay(pdMS_TO_TICKS(2000));
  xSemaphoreGive(xBinSemaphore);
}

void vTaskADC(void *pvParameters){
  int adcValue;
  while(1){
    /*lock the task waiting for semaphore*/
    xSemaphoreTake(xBinSemaphore, portMAX_DELAY);
    /*doing an analog reading everytime the semaphore is free to go*/
    adcValue = analogRead(ANALOG_INPUT);
    Serial.println("ADC Value: "+ String(adcValue)+"\n");
  }
}