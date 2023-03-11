/*
 * WatchdogTimer.h
 *
 *  Created on: Jan 2, 2023
 *      Author: Eric Mintz
 *
 * Watchdog timer. The timer manages a blink task, that is bound during
 * construction. The task is resumed when the timer fires, and is suspended
 * when the timer is running. Note that this is not a task.
 */

#include "Arduino.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "freertos/timers.h"

#include "BlinkTask.h"

#ifndef WATCHDOGTIMER_H_
#define WATCHDOGTIMER_H_

#define WATCHDOG_TIMER_NUMBER_OF_STATES 6
#define WATCHDOG_TIMER_NUMBER_OF_EVENTS 2

/**
 * A watchdog timer that resumes a task when it fires and suspends a task
 * when it is not firing.
 */
class WatchdogTimer {
public:
  enum State {
    CREATED,
    STARTING,
    RESETTING,
    HAS_RESET,
    EXPIRING,
    HAS_EXPIRED,
  };

  enum Event {
    RESET,
    EXPIRE,
  };

private:
  TickType_t timeout_in_ticks;
  State state;
  BlinkTask *blink_task;
  TimerHandle_t h_timer;
  QueueHandle_t h_connection_status_queue;
  const char *timer_name;

  static void on_timer_expired(TimerHandle_t h_timer);

  /**
   * Change state and perform the new state's action
   */
  void transition(Event event);
public:

  /**
   * Construct and configure a watchdog timer
   *
   * Parameters                Contents
   * ------------------------  -----------------------------------------
   * blink_task                Blink task to resume and suspend.
   * timer_name                Timer name for error and exception
   *                           messages
   * timeout_in_milliseconds   Expiration time in milliseconds. The timer
   *                           fires if it is not reset within this time.
   */
  WatchdogTimer(
    const char *timer_name,
    BaseType_t timout_in_milliseconds,
    BlinkTask *blink_task);

  /**
   * Destructor for the sake of completeness. It is never invoked.
   */
  virtual ~WatchdogTimer();

  /**
   * Start the timer. Called once at startup.
   */
  void start(QueueHandle_t h_connection_status_queue);

  /**
   * Restart the countdown.
   */
  void reset();
};

#endif /* WATCHDOGTIMER_H_ */
