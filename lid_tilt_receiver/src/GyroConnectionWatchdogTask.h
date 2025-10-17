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
#include "ConnectionStatus.h"
#include "Resettable.h"

#include "FreeRunningTimerH.h"
#include "PullQueueHT.h"
#include "Task.h"
#include "VoidFunction.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

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

  class OnTimerExpired : public VoidFunction {
    friend class GyroConnectionWatchdogTask;
    GyroConnectionWatchdogTask& watchdog_task_;

    OnTimerExpired(GyroConnectionWatchdogTask& watchdog_task) :
      watchdog_task_(watchdog_task) {
    }

    virtual ~OnTimerExpired() = default;

    virtual void apply(void) {
      watchdog_task_.expire();
    }
  };

  PullQueueHT<ConnectionStatusMessage>& connection_status_queue_;
  OnTimerExpired expiration_;
  FreeRunningTimerH watchdog_timer_;
  State state_;
  PullQueueHT<EventMessage_t> timer_event_queue_;

  static EventMessage_t EXPIRE_MESSAGE;
  static EventMessage_t RESET_MESSAGE;

  static void on_timer_expired(TimerHandle_t h_timer);

public:
  GyroConnectionWatchdogTask(
      PullQueueHT<ConnectionStatusMessage>& connection_status_queue);
  virtual ~GyroConnectionWatchdogTask();

  void expire(void);

  virtual void reset(void);

  TaskHandle_t start();

  virtual void task_loop(void);
};

#endif /* GYROCONNECTIONWATCHDOGTASK_H_ */
