/* This is an example of how to use software timers on FreeRTOS.
* 2 timers are created. Timer1 is an auto reload timer, and inverts 
* the LED_A on every 1 second. Timer2 is a one shot timer, called by task1
* everytime the button is pressed. Timer2 turns off the LED_B 10 seconds after
* its activation.
* code by: Rafael Sombra.
*/

#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"

#define LED_A 2
#define LED_B 14
#define BUTTON 12

TaskHandle_t xTask1;
TimerHandle_t xTimer1, xTimer2;

// put function declarations here:
void vTask1(void *pvParameters);
void vCallBackTimer1(TimerHandle_t xTimer);
void vCallBackTimer2(TimerHandle_t xTimer);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(LED_A, OUTPUT);
  pinMode(LED_B, OUTPUT);
  pinMode(BUTTON, INPUT_PULLUP);

  xTimer1 = xTimerCreate("TIMER1", pdMS_TO_TICKS(1000), pdTRUE, 0, vCallBackTimer1);
  xTimer2 = xTimerCreate("TIMER2", pdMS_TO_TICKS(10000), pdFALSE, 0, vCallBackTimer2);

  xTaskCreate(vTask1, "TASK1", configMINIMAL_STACK_SIZE +1024, NULL, 1, &xTask1);
  xTimerStart(xTimer1, 0);

}

void loop() {
  // put your main code here, to run repeatedly:
  vTaskDelay(pdMS_TO_TICKS(1000));
}

// put function definitions here:
void vTask1(void *pdParameters) {
  uint8_t ucDebouncingTime = 0;
  while(1) 
  {
    if ((LOW == digitalRead(BUTTON)) && 
      (pdFALSE == xTimerIsTimerActive(xTimer2))) 
    {
      ucDebouncingTime++;
      if (10 <= ucDebouncingTime) 
      {
        ucDebouncingTime = 0;
        digitalWrite(LED_B, HIGH);
        /* start counting 10sec to turn off LED_B. */
        xTimerStart(xTimer2, 0);
        /* stop bliking LED_A. */
        xTimerStop(xTimer1, 0);
        Serial.println("Starting timer 2 ...");
      }
    }
    else 
    {
      ucDebouncingTime = 0;
    }
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

void vCallBackTimer1(TimerHandle_t xTimer) {
  digitalWrite(LED_A, !digitalRead(LED_A));
}

void vCallBackTimer2(TimerHandle_t xTimer) {
  digitalWrite(LED_B, LOW);
  /* Start blinking the LED_A again. */
  xTimerStart(xTimer1, 0);
}