/*
 * DeliveryLedTask.h
 *
 *  Created on: Feb 20, 2023
 *      Author: Eric Mintz
 *
 * Illuminates the delivery indicate LED as directed by incoming commands.
 */

#ifndef DELIVERYLEDTASK_H_
#define DELIVERYLEDTASK_H_

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "DeliveryLEDIlluminationStatus.h"
#include "PullQueueHT.h"
#include "Task.h"

class DeliveryLedTask :
  public Task {
  PullQueueHT<LedIlluminationMessage>& delivery_led_illumination_queue_;
  const uint8_t led_pin;  // The GPIO pin that controls the LED.
  const uint16_t on_time_ms;  // The time to hold the LED on when blinking
  const uint16_t off_time_ms;  // The time to hold the LED off when blinking.

public:
  DeliveryLedTask(
      PullQueueHT<LedIlluminationMessage>& delivery_led_illumination_queue,
      uint8_t led_pin,
      uint16_t on_time_ms,
      uint16_t off_time_ms);
  virtual ~DeliveryLedTask();

  TaskHandle_t start(void);

  virtual void task_loop();
};

#endif /* DELIVERYLEDTASK_H_ */
