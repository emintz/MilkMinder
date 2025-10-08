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
#include "freertos/queue.h"
#include "freertos/task.h"

#include "Task.h"

class DeliveryLedTask :
  public Task {
  const uint8_t led_pin;  // The GPIO pin that controls the LED.
  const uint16_t on_time_ms;  // The time to hold the LED on when blinking
  const uint16_t off_time_ms;  // The time to hold the LED off when blinking.
  QueueHandle_t h_delivery_led_illumination_queue;

public:
  DeliveryLedTask(
      uint8_t led_pin,
      uint16_t on_time_ms,
      uint16_t off_time_ms);
  virtual ~DeliveryLedTask();

  TaskHandle_t start(QueueHandle_t h_delivery_led_queue);

  virtual void task_loop();
};

#endif /* DELIVERYLEDTASK_H_ */
