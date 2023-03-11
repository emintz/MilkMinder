/*
 * DisconnectedLedTask.h
 *
 *  Created on: Feb 20, 2023
 *      Author: Eric Mintz
 *
 * Blinks the disconnected LED when running.
 */

#ifndef DISCONNECTEDLEDTASK_H_
#define DISCONNECTEDLEDTASK_H_

#include "Arduino.h"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#include "Task.h"

class DisconnectedLedTask :
  public Task {
  QueueHandle_t h_alarm_event_queue;
  TaskHandle_t h_task;
  const uint8_t led_pin;

public:
  DisconnectedLedTask(
      uint8_t led_pin);
  virtual ~DisconnectedLedTask();

  TaskHandle_t start(QueueHandle_t h_alarm_event_queue);

  /**
   * Disables (suspends) the task and turns off the delivery LED.
   */
  void disable();

  /**
   * Enables the task, causing the LED to blink.
   */
  void enable();

  virtual void task_loop();
};

#endif /* DISCONNECTEDLEDTASK_H_ */
