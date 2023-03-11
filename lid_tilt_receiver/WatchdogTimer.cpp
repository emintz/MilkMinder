/*
 * WatchdogTimer.cpp
 *
 *  Created on: Jan 2, 2023
 *      Author: Eric Mintz
 */

#include "WatchdogTimer.h"

#include <stdlib.h>

#include "ConnectionStatus.h"

static ConnectionStatusMessage CONNECTION_DOWN = { CONNECTION_STATUS_DOWN };
static ConnectionStatusMessage CONNECTION_UP = { CONNECTION_STATUS_UP };

static const WatchdogTimer::State TRANSITION_TABLE
    [WATCHDOG_TIMER_NUMBER_OF_STATES]
    [WATCHDOG_TIMER_NUMBER_OF_EVENTS] =
  {
    {  // CRREATED
      WatchdogTimer::STARTING,  // RESET
      WatchdogTimer::EXPIRING,  // EXPIRED, should not happen
    },
    {	// STARTING
      WatchdogTimer::RESETTING,  // RESET
      WatchdogTimer::EXPIRING,  // EXPIRED
    },
    {  // RESETTING
      WatchdogTimer::HAS_RESET,  // RESET
      WatchdogTimer::EXPIRING,  // EXPIRE
    },
    {  // HAS_RESET
      WatchdogTimer::HAS_RESET,  // RESET
      WatchdogTimer::EXPIRING,  // EXPIRE
    },
    {  //  EXPIRING
      WatchdogTimer::RESETTING,  // RESET
      WatchdogTimer::HAS_EXPIRED,  // EXPIRE
    },
    {  // HAS_EXPIRED
      WatchdogTimer::RESETTING,  // RESET
      WatchdogTimer::HAS_EXPIRED,  // EXPIRE
    },
  };

  WatchdogTimer::WatchdogTimer(
      const char *timer_name,
      BaseType_t timeout_in_milliseconds,
      BlinkTask *blink_task) :
    timeout_in_ticks(pdMS_TO_TICKS(timeout_in_milliseconds)),
    blink_task(blink_task),
    state(CREATED),
    h_connection_status_queue(NULL),
    h_timer(NULL),
    timer_name(timer_name) {
  }

WatchdogTimer::~WatchdogTimer() {
}

void WatchdogTimer::on_timer_expired(TaskHandle_t h_timer) {
  ((WatchdogTimer *) pvTimerGetTimerID(h_timer))
      ->transition(WatchdogTimer::EXPIRE);
}

void WatchdogTimer::transition(Event event) {
  state = TRANSITION_TABLE[state][event];
  switch (state) {
    case CREATED:
      // Nothing to do
      break;
    case STARTING:
      xTimerStart(h_timer, 0);
      break;
    case RESETTING:
      Serial.println("WIFI timer reset");
      xTimerReset(h_timer, 0);
      xQueueSendToBack(
          h_connection_status_queue,
          &CONNECTION_UP,
          pdMS_TO_TICKS(10));
      break;
    case HAS_RESET:
      xTimerReset(h_timer, 0);
      break;
    case EXPIRING:
      Serial.println("WIFI timeout.");
      xQueueSendToBack(
          h_connection_status_queue,
          &CONNECTION_DOWN,
          pdMS_TO_TICKS(10));
      break;
    case HAS_EXPIRED:
      // Nothing to do
      break;
  }
}

void WatchdogTimer::reset() {
  transition(RESET);
}

void WatchdogTimer::start(QueueHandle_t h_communications_event_queue) {
  Serial.print("WatchdogTimer::start connection event handle: ");
  Serial.println((uint32_t) h_communications_event_queue, HEX);

  h_connection_status_queue = h_communications_event_queue;

  h_timer = xTimerCreate(
    timer_name,
    timeout_in_ticks,
    pdTRUE,
    this,
    on_timer_expired);
}
