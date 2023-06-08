/*
 * GyroConnectionWatchdogTask.h
 *
 *  Created on: May 21, 2023
 *      Author: Eric Mintz
 *
 * Timeout task for the ESPNow connection, a free running timer that sends a
 * timeout notification when it expires. The ESPNow handler resets the timer
 * whenever it receives a command, so if the gyroscope reader is connected,
 * the timer never expires.
 */

#ifndef GYROCONNECTIONWATCHDOGTASK_H_
#define GYROCONNECTIONWATCHDOGTASK_H_

#include "Arduino.h"
#include "BlinkTask.h"
#include "Resettable.h"
#include "Task.h"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "freertos/timers.h"

class GyroConnectionWatchdogTask : public Task, public Resettable {
  public:
  enum State {
    CREATED,
    STARTING,
    RESETTING,
    HAS_RESET,
    EXPIRING,
    HAS_EXPIRED,
    GYRO_WATCHDOG_NUMBER_OF_STATES,
  };

  enum Event {
    RESET,
    EXPIRE,
    GYRO_WATCHDOG_NUMBER_OF_EVENTS,
  };

private:
  typedef struct {
    Event event;
  } EventMessage_t;

  BlinkTask *blink_task;

  State state;
  TimerHandle_t h_timer;
  QueueHandle_t h_connection_status_queue;
  QueueHandle_t h_timer_event_queue;

  static EventMessage_t EXPIRE_MESSAGE;
  static EventMessage_t RESET_MESSAGE;

  static void on_timer_expired(TimerHandle_t h_timer);

public:
  GyroConnectionWatchdogTask(
      BlinkTask *blink_task);
  virtual ~GyroConnectionWatchdogTask();

  void expire(void);

  virtual void reset(void);

  TaskHandle_t start(QueueHandle_t h_communications_event_queue);

  virtual void task_loop(void);
};

#endif /* GYROCONNECTIONWATCHDOGTASK_H_ */
