/*
 * OneShotTimerWithAction.cpp
 *
 *  Created on: Mar 28, 2023
 *      Author: Eric Mintz
 */

#include "OneShotTimerWithAction.h"

#include <string.h>

OneShotTimerWithAction::State OneShotTimerWithAction::TRANSITION_TABLE
    [OST_STATE_NUMBER_OF_STATES][OST_EVENT_NUMBER_OF_EVENTS] =
{
    { // EXPIRED state
      OST_STATE_NUMBER_OF_STATES,  // OST_EVENT_EXPIRE, Expired, do nothing.
      OST_STATE_NUMBER_OF_STATES,  // OST_EVENT_RESET, Reset, do nothing.
      OST_STATE_STARTING,  // OST_EVENT_START, Start, start up
      OST_STATE_NUMBER_OF_STATES,  // OST_EVENT_STOP, Stop, do nothing
    },
    {  // RESETTING state
      OST_STATE_EXPIRED,  // OST_EVENT_EXPIRE, Expired, invoke the action
      OST_STATE_RESETTING,  // OST_EVENT_RESET, Reset, do the reset.
      OST_STATE_STARTING,  // OST_EVENT_START, Start, restart the timer
      OST_STATE_STOPPING,  // OST_EVENT_STOP, Stop, stop the timer.
    },
    {  // STARTING state
      OST_STATE_EXPIRED,  //OST_EVENT_EXPIRE,  Expired, invoke the action
      OST_STATE_RESETTING,  // OST_EVENT_RESET, Reset, do the reset.
      OST_STATE_STARTING,  // OST_EVENT_START, Start, restart the timer
      OST_STATE_STOPPING,  // OST_EVENT_STOP, Stop, stop the timer.
    },
    { // STOPPED state
      OST_STATE_NUMBER_OF_STATES,  // OST_EVENT_EXPIRE, Expired, do nothing
      OST_STATE_NUMBER_OF_STATES,  // ROST_EVENT_RESET, reset, do nothing -- why we have reset.
      OST_STATE_STARTING,  // OST_EVENT_START, Start, start the timer.
      OST_STATE_NUMBER_OF_STATES,  // OST_EVENT_STOP, Stop, do nothing
    },
    {  // FAILED state
      OST_STATE_EXPIRED,  // OST_EVENT_EXPIRE, Expired. Perform action & exit error state
      OST_STATE_NUMBER_OF_STATES,  // OST_EVENT_RESET, Reset, do nothing
      OST_STATE_NUMBER_OF_STATES,  // OST_EVENT_START, Start, do nothing
      OST_STATE_NUMBER_OF_STATES,  // OST_EVENT_STOP, Stop, do nothing
    },
};

void OneShotTimerWithAction::timer_callback(TimerHandle_t timer_handle) {
//  Serial.println("Countown timer expired.");
  ((OneShotTimerWithAction *) pvTimerGetTimerID(timer_handle))->
      on_timer_expired();
}

OneShotTimerWithAction::OneShotTimerWithAction(
    const char *name,
    Action *action) :
        action(action),
        timer_name(name),
        current_state(OST_STATE_STOPPING) {
  memset(&mutex_buffer, 0, sizeof(mutex_buffer));
  mutex_handle = xSemaphoreCreateMutexStatic(&mutex_buffer);
  memset(&timer_buffer, 0, sizeof(timer_buffer));
  create_timer();
}

OneShotTimerWithAction::~OneShotTimerWithAction() {
  ((OneShotTimerWithAction *) pvTimerGetTimerID(timer_handle))->
      on_timer_expired();
}

void OneShotTimerWithAction::on_timer_expired() {
  transition(OST_EVENT_EXPIRED, 0);
}

BaseType_t OneShotTimerWithAction::create_timer(void) {
  // Since we set the timer period whenever we start the timer,
  // we set it to a dummy value now.
  timer_handle = xTimerCreateStatic(
      timer_name,
      1,
      pdFALSE,
      this,
      timer_callback,
      &timer_buffer);
  return timer_handle ? pdPASS : pdFAIL;
}

BaseType_t OneShotTimerWithAction::transition(
    Event event, TickType_t new_timeout_in_ticks) {
  xSemaphoreTake(mutex_handle, portMAX_DELAY);
  BaseType_t result = pdPASS;
  State maybe_new_state = TRANSITION_TABLE[current_state][event];
  if (maybe_new_state != OST_STATE_NUMBER_OF_STATES) {
    switch (current_state = maybe_new_state) {
    case OST_STATE_EXPIRED:
      action->run();
      break;
    case OST_STATE_RESETTING:
      result = xTimerReset(timer_handle, 0);
      break;
    case OST_STATE_STARTING:
      result = xTimerChangePeriod(timer_handle, new_timeout_in_ticks, 0);
      if (result) {
        result = xTimerStart(timer_handle, 0);
      }
      break;
    case OST_STATE_STOPPING:
      result = xTimerStop(timer_handle, 0);
      break;
    case OST_STATE_FAILED:
      break;
    case OST_STATE_NUMBER_OF_STATES:
      // Should never happen
      break;
    }
  }
  xSemaphoreGive(mutex_handle);
  return result;
}

BaseType_t OneShotTimerWithAction::force_clear(void) {
  BaseType_t result = xSemaphoreTake(mutex_handle, portMAX_DELAY);
  if (mutex_handle != NULL && result == pdPASS) {
    result = xTimerDelete(timer_handle, portMAX_DELAY);
    mutex_handle = NULL;
  }
  result == pdPASS ? create_timer() : result;
  current_state = result == pdPASS ? OST_STATE_STOPPING : OST_STATE_FAILED;
  xSemaphoreGive(timer_handle);
  return result;
}

BaseType_t OneShotTimerWithAction::reset(void) {
  return transition(OST_EVENT_RESET, 0);
}

BaseType_t OneShotTimerWithAction::start(TickType_t countdown_time) {
  return transition(OST_EVENT_START, countdown_time);
}

BaseType_t OneShotTimerWithAction::stop(void) {
  return transition(OST_EVENT_STOP, 0);
}
