/*
 * OneShotTimerWithAction.h
 *
 *  Created on: Mar 28, 2023
 *      Author: Eric Mintz
 */

#ifndef ONESHOTTIMERWITHACTION_H_
#define ONESHOTTIMERWITHACTION_H_

#include "Arduino.h"

#include "Action.h"

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/timers.h"

class OneShotTimerWithAction {

  enum Event {
    OST_EVENT_EXPIRED,
    OST_EVENT_RESET,
    OST_EVENT_START,
    OST_EVENT_STOP,
    OST_EVENT_NUMBER_OF_EVENTS,  // MUST be last
  };

  enum State {
    OST_STATE_EXPIRED,
    OST_STATE_RESETTING,
    OST_STATE_STARTING,
    OST_STATE_STOPPING,
    OST_STATE_FAILED,
    OST_STATE_NUMBER_OF_STATES,  // MUST be last, does nothing.
  };

  static State TRANSITION_TABLE
      [OST_STATE_NUMBER_OF_STATES][OST_EVENT_NUMBER_OF_EVENTS];

  Action *action;

  const char *timer_name;
  SemaphoreHandle_t mutex_handle;
  StaticSemaphore_t mutex_buffer;
  TimerHandle_t timer_handle;
  StaticTimer_t timer_buffer;
  State current_state;

  static void timer_callback(TimerHandle_t timer_handle);

  BaseType_t create_timer(void);

  void on_timer_expired();

  BaseType_t transition(Event event, TickType_t new_timeout_in_ticks);

public:
  OneShotTimerWithAction(
      const char *name,
      Action *action);
  virtual ~OneShotTimerWithAction();

  bool status(void) {
    return mutex_handle != NULL;
  }

  BaseType_t force_clear(void);

  BaseType_t reset();

  BaseType_t start(TickType_t countdown_time);

  BaseType_t stop(void);
};

#endif /* ONESHOTTIMERWITHACTION_H_ */
