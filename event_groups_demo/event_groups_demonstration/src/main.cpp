/*
* Demonstrative code for event group feature from FreeRTOS.
* Here we have a timer (xTimer1) triggering a callback func every 1 sec.
* This callback func will set the event bit for resuming task1 after 
* 5sec, and set the event bit for task2 after 10sec. Both setted after 
* 15 seconds. 
* When the respective event bit is set to 1, the task is resumed.
* loop() only inverts the board LED every sec.
* Serial is used to output status.
* coded by: Rafael Sombra.
*/

#include <Arduino.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "freertos/event_groups.h"

#define LED 2
/* event mapping. */
#define TASK1_FLAG (1<<0) 
#define TASK2_FLAG (1<<1)

TaskHandle_t xTask1Handle, xTask2Handle;
TimerHandle_t xTimer1;
EventGroupHandle_t xMyEventGroup;
int sec_counter = 0;

void vTask1(void *pvParameters);
void vTask2(void *pvParameters);
void vCallBackTimer1(TimerHandle_t pxTimer);

/* setup code to run once. */
void setup() 
{
  Serial.begin(9600);
  pinMode(LED, OUTPUT);

  xMyEventGroup = xEventGroupCreate(); 
  /* triggering timer event every 1 sec. */
  xTimer1 = xTimerCreate("MY_TIMER", pdMS_TO_TICKS(1000), pdTRUE, 0, vCallBackTimer1);
  xTaskCreate(vTask1, "TASK1", configMINIMAL_STACK_SIZE +1024, NULL, 1, &xTask1Handle);
  xTaskCreate(vTask2, "TASK2", configMINIMAL_STACK_SIZE +1024, NULL, 1, &xTask2Handle);

  xTimerStart(xTimer1, 0);
}

/* main code to run repeatedly. */
void loop() 
{
  digitalWrite(LED, !digitalRead(LED));
  vTaskDelay(500); 
}

/* function definitions here: */
void vTask1(void *pvParameters)
{
  EventBits_t xBits;
  /* waiting event to resume the task.*/
  while(1)
  {
    xBits = xEventGroupWaitBits(xMyEventGroup, TASK1_FLAG, pdTRUE, pdTRUE, portMAX_DELAY);
    Serial.println("Task1 resumed!!!");
  }
}

void vTask2(void *pvParameters)
{
  EventBits_t xBits;
  /* waiting event to resume the task.*/
  while(1)
  {
    xBits = xEventGroupWaitBits(xMyEventGroup, TASK2_FLAG, pdTRUE, pdTRUE, portMAX_DELAY);
    Serial.println("Task2 resumed!!!");
  }
}

/* dummie callback func to set the event bits for task1 and task2.
* it will resume task1 after 5sec, and resume task2 after 10sec.
*/
void vCallBackTimer1(TimerHandle_t pxTimer)
{
  sec_counter++;
  if (5 == sec_counter) {
    xEventGroupSetBits(xMyEventGroup, TASK1_FLAG);
  }
  else if (10 == sec_counter) {
    xEventGroupSetBits(xMyEventGroup, TASK2_FLAG);
  }
  else if (15 == sec_counter) {
    xEventGroupSetBits(xMyEventGroup, TASK1_FLAG|TASK2_FLAG);
    sec_counter = 0; /* restarting counting. */
  }
}