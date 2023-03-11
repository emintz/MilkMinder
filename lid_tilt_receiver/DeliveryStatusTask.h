/*
 * DeliveryStatusTask.h
 *
 *  Created on: Feb 23, 2023
 *      Author: Eric Mintz
 *
 * Task that translates delivery status to desured user interface behavior.
 * The task reponds to delivery events by sending commands to the LCD,
 * delivery LED, and alarm.
 */

#ifndef DELIVERYSTATUSTASK_H_
#define DELIVERYSTATUSTASK_H_

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#include "DeliveryEvent.h"
#include "DeliveryStatus.h"
#include "TimeTask.h"
#include "Task.h"

class DeliveryStatusTask :
  public Task {

  static const DeliveryStatus
      TRANSITION_TABLE[DELIVERY_STATUS_COUNT][DELIVERY_EVENT_COUNT];

  DeliveryStatus state;

  TimeTask *time_task;

  QueueHandle_t h_delivery_event_queue;
  QueueHandle_t h_delivery_led_illumination_queue;
  QueueHandle_t h_alarm_event_queue;
  QueueHandle_t h_display_command_queue;

  /**
   * Sound an transmitter failure panic alarm.
   */
  void fail();

  /**
   * Indicate that the lid is open.
   */
  void lid_is_open();

  /**
   * Calm down, everything is OK.
   */
  void quiesce();

public:
  DeliveryStatusTask(TimeTask *time_task);
  virtual ~DeliveryStatusTask();

  TaskHandle_t start(
      QueueHandle_t h_delivery_event_queue,
      QueueHandle_t h_delivery_led_illumination_queue,
      QueueHandle_t h_alarm_event_queue,
      QueueHandle_t h_display_command_queue);

  virtual void task_loop();
};

#endif /* DELIVERYSTATUSTASK_H_ */
