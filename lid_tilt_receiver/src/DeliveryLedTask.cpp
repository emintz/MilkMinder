/*
 * DeliveryLedTask.cpp
 *
 *  Created on: Feb 20, 2023
 *      Author: Eric Mintz
 */
#include "DeliveryLedTask.h"

#include "Arduino.h"

#include "DeliveryLEDIlluminationStatus.h"

DeliveryLedTask::DeliveryLedTask(
    uint8_t led_pin,
    uint16_t on_time_ms,
    uint16_t off_time_ms) :
    Task("Delivery LED",
        2048,
        5),
    led_pin(led_pin),
    on_time_ms(on_time_ms),
    off_time_ms(off_time_ms),
    h_delivery_led_illumination_queue(NULL) {
}

DeliveryLedTask::~DeliveryLedTask() {
}

TaskHandle_t DeliveryLedTask::start(
    QueueHandle_t h_delivery_led_illumination_queue) {
  this->h_delivery_led_illumination_queue = h_delivery_led_illumination_queue;
  return create_and_start_task();
}

void DeliveryLedTask::task_loop() {
  LedIlluminationMessage illumination;
  for (;;) {
    if (xQueueReceive(
        h_delivery_led_illumination_queue,
        &illumination,
        portMAX_DELAY)) {
      switch (illumination.illumination) {
      case DELIVERY_LED_OFF:
        digitalWrite(led_pin, LOW);
        break;
      case DELIVERY_LED_BLINK:
        while (!xQueuePeek(
            h_delivery_led_illumination_queue,
            &illumination,
            0)) {
          digitalWrite(led_pin, HIGH);
          xQueuePeek(
            h_delivery_led_illumination_queue,
            &illumination,
            pdMS_TO_TICKS(on_time_ms));
          digitalWrite(led_pin, LOW);
          xQueuePeek(
            h_delivery_led_illumination_queue,
            &illumination,
            pdMS_TO_TICKS(off_time_ms));
        }
        break;
      case DELIVERY_LED_ON:
        digitalWrite(led_pin, HIGH);
        break;
      }
    }
  }
}
