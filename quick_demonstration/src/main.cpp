/* This project intends to bring a quick demonstration of some 
* FreeRTOS features, such as task creation, reading ADC values from
* an input, memory management, and response to ISRs.
* code by: Rafael Sombra.
*/


#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

/* pin mapping.*/
#define LED_A 2
#define LED_B 14
#define BUTTON 12
#define ANALOG_INPUT 34

TaskHandle_t xTask1Handle = NULL;
QueueHandle_t xQueue = NULL;
TaskHandle_t xTaskMonitorHandle = NULL;

//function declarations:
void vTreatButtonEvent();
void vTaskMonitor(void *vpParameters);

void setup() {
  Serial.begin(9600);
  pinMode(LED_A, OUTPUT);
  pinMode(LED_B, OUTPUT);
  pinMode(BUTTON, INPUT_PULLUP);
  BaseType_t xReturned;
  xQueue = xQueueCreate(5, sizeof(int));
  
  if (NULL == xQueue)
  {
    Serial.println("It was not possible to create the queue! Blocking exec!!!");
    while(1);
  }
  xReturned = xTaskCreatePinnedToCore(vTaskMonitor, "TaskMonitor", 
    configMINIMAL_STACK_SIZE +1024, NULL, 2, &xTaskMonitorHandle, APP_CPU_NUM);
  if (pdFAIL == xReturned) 
  {
    Serial.println("Failed to create the monitoring task!!! Blocking exec!!!");
    while(1);
  }

  /* ISR from the button press, triggered by the falling edge of the signal.*/
  attachInterrupt(digitalPinToInterrupt(BUTTON), vTreatButtonEvent, FALLING);
}

void loop() {
  digitalWrite(LED_A, !digitalRead(LED_A));
  vTaskDelay(pdMS_TO_TICKS(1000));
  }

// Function definitions:

/*Callback func to read the analog input everytime the button is pressed.
* The reading values are sent to a queue the be treated by another task. */
void vTreatButtonEvent() {
  int xSensorReading = 0;
  BaseType_t xHigherPriorityTaskWoken = pdTRUE;
  xSensorReading = analogRead(ANALOG_INPUT);
  if (errQUEUE_FULL == xQueueSendFromISR(xQueue, &xSensorReading, &xHigherPriorityTaskWoken))
  {
    Serial.println("The queue is full right now!!!");
  }
  else Serial.println("Value sent to the Q: " +String(xSensorReading));
  // mudar estado do LED_B
  // timer para desligar o LED_B
}

/* Reads the analogic input from the pin passed by parameter,
* and prints the value to the Serial. Values will be showed on terminal. */
void vReadAnalogInput(int xInputPin){
  Serial.println("ADC Value: " +String(analogRead(xInputPin)));
}

void vTaskMonitor(void *vpParameters){
  int xReceived;
  while (1)
  {
    if (pdTRUE == xQueueReceive(xQueue, &xReceived, pdMS_TO_TICKS(1000)))
    {
      Serial.println("Received value: " +String(xReceived));
      vTaskDelay(pdMS_TO_TICKS(500));
    }
  }
  
}
