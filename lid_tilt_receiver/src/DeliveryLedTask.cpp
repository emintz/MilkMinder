/*
 * DeliveryLedTask.cpp
 *
 *  Created on: Feb 20, 2023
 *      Author: Eric Mintz
 */
#include "DeliveryLedTask.h"

#include "Arduino.h"

DeliveryLedTask::DeliveryLedTask(
    PullQueueHT<LedIlluminationMessage>& delivery_led_illumination_queue,
    uint8_t led_pin,
    uint16_t on_time_ms,
    uint16_t off_time_ms) :
    Task("Delivery LED",
        2048,
        5),
        delivery_led_illumination_queue_(delivery_led_illumination_queue),
    led_pin(led_pin),
    on_time_ms(on_time_ms),
    off_time_ms(off_time_ms) {
}

DeliveryLedTask::~DeliveryLedTask() {
}

TaskHandle_t DeliveryLedTask::start() {
  return create_and_start_task();
}

void DeliveryLedTask::task_loop() {
  LedIlluminationMessage illumination;
  for (;;) {
    if (delivery_led_illumination_queue_.pull_message(&illumination)) {
      switch (illumination.illumination) {
      case DELIVERY_LED_OFF:
        digitalWrite(led_pin, LOW);
        break;
      case DELIVERY_LED_BLINK:
        while (!delivery_led_illumination_queue_.peek_message(
            &illumination, 0)) {
          digitalWrite(led_pin, HIGH);
          delivery_led_illumination_queue_.peek_message(
              &illumination, pdMS_TO_TICKS(on_time_ms));
          digitalWrite(led_pin, LOW);
          delivery_led_illumination_queue_.peek_message(
              &illumination, pdMS_TO_TICKS(off_time_ms));
        }
        break;
      case DELIVERY_LED_ON:
        digitalWrite(led_pin, HIGH);
        break;
      }
    }
  }
}
