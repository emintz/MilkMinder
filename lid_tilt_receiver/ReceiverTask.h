/*
 * ReceiverTask.h
 *
 *  Created on: Feb 14, 2023
 *      Author: Eric Mintz
 *
 * Listens to the communications queue and drives the user interface. The
 * user interface includes the alarm (a.k.a.) beeper, LCD, and LEDS.
 */

#ifndef RECEIVERTASK_H_
#define RECEIVERTASK_H_

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#include "Task.h"
#include "TimeTask.h"
#include "WatchdogTimer.h"

class ReceiverTask :
    public Task {
  enum LidPosition {
    OPEN,
    CLOSED,
    RCV_LID_POSITION_COUNT,
  };

  QueueHandle_t h_communications_queue;  // Incoming communications events
  QueueHandle_t h_lid_position_report_queue;  // Delivery events for MilkArrivalTask

  const TimeTask *time_task;
  WatchdogTimer * watchdog_timer;

  static void on_esp_now_received(
      const uint8_t *mac,
      const uint8_t *received_data,
      int len);

  virtual void task_loop();

public:
  ReceiverTask(
      TimeTask *time_task,
      WatchdogTimer *watchdog_timer);
  virtual ~ReceiverTask();

  static bool begin();

  TaskHandle_t start(
      QueueHandle_t h_communications_queue,
      QueueHandle_t h_lid_position_report_queue);
};

#endif /* RECEIVERTASK_H_ */
