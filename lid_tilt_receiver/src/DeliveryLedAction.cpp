/*
 * DeliveryLedAction.cpp
 *
 *  Created on: Feb 20, 2023
 *      Author: Eric Mintz
 */
#include <src/DeliveryLedAction.h>

#include "Arduino.h"

DeliveryLedAction::DeliveryLedAction(
    PullQueueHT<LedIlluminationMessage>& delivery_led_illumination_queue,
    uint8_t led_pin,
    uint16_t on_time_ms,
    uint16_t off_time_ms) :
      delivery_led_illumination_queue_(delivery_led_illumination_queue),
      led_pin_(led_pin),
      on_time_ms_(on_time_ms),
      off_time_ms_(off_time_ms) {
}

DeliveryLedAction::~DeliveryLedAction() {
}

void DeliveryLedAction::run() {
  LedIlluminationMessage illumination;
  for (;;) {
    if (delivery_led_illumination_queue_.pull_message(&illumination)) {
      switch (illumination.illumination) {
      case DELIVERY_LED_OFF:
        digitalWrite(led_pin_, LOW);
        break;
      case DELIVERY_LED_BLINK:
        while (!delivery_led_illumination_queue_.peek_message(
            &illumination, 0)) {
          digitalWrite(led_pin_, HIGH);
          delivery_led_illumination_queue_.peek_message(
              &illumination, pdMS_TO_TICKS(on_time_ms_));
          digitalWrite(led_pin_, LOW);
          delivery_led_illumination_queue_.peek_message(
              &illumination, pdMS_TO_TICKS(off_time_ms_));
        }
        break;
      case DELIVERY_LED_ON:
        digitalWrite(led_pin_, HIGH);
        break;
      }
    }
  }
}
