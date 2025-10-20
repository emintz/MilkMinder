/*
 * DeliveryLedAction.h
 *
 *  Created on: Feb 20, 2023
 *      Author: Eric Mintz
 *
 * Illuminates the delivery indicate LED as directed by incoming commands.
 * The LED can be off, blinking, or on.
 */

#ifndef DELIVERYLEDACTION_H_
#define DELIVERYLEDACTION_H_

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "DeliveryLEDIlluminationStatus.h"
#include "PullQueueHT.h"
#include "TaskAction.h"

class DeliveryLedAction : public TaskAction {
  PullQueueHT<LedIlluminationMessage>& delivery_led_illumination_queue_;
  const uint8_t led_pin_;  // The GPIO pin that controls the LED.
  const uint16_t on_time_ms_;  // The time to hold the LED on when blinking
  const uint16_t off_time_ms_;  // The time to hold the LED off when blinking.

public:
  /**
   * Constructor
   *
   * Parameters                      Contents
   * ------------------------------- -------------------------------------
   * delivery_led_illumination_queue Input queue, provides commands
   * led_pin                         The pin to illuminate
   * on_time_ms                      The length of time to illuminate the
   *                                 LED when blinking
   * off_time_ms                     The length of time to keep the LED off
   *                                 when blinking
   */
  DeliveryLedAction(
      PullQueueHT<LedIlluminationMessage>& delivery_led_illumination_queue,
      uint8_t led_pin,
      uint16_t on_time_ms,
      uint16_t off_time_ms);
  virtual ~DeliveryLedAction();

  /**
   * Task logic
   */
  virtual void run() override;
};

#endif /* DELIVERYLEDACTION_H_ */
