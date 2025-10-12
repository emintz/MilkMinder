/*
 * GyroConnectionWatchdogTask.cpp
 *
 *  Created on: May 21, 2023
 *      Author: Eric Mintz
 */

#include "GyroConnectionWatchdogTask.h"


static ConnectionStatusMessage CONNECTION_DOWN = { CONNECTION_STATUS_DOWN };
static ConnectionStatusMessage CONNECTION_UP = { CONNECTION_STATUS_UP };

#define STACK_DEPTH 2048
#define PRIORITY 2

GyroConnectionWatchdogTask::EventMessage_t
    GyroConnectionWatchdogTask::EXPIRE_MESSAGE = {
      GyroConnectionWatchdogTask::EXPIRE,
};
GyroConnectionWatchdogTask::EventMessage_t
    GyroConnectionWatchdogTask::RESET_MESSAGE = {
      GyroConnectionWatchdogTask::RESET,
};

static const GyroConnectionWatchdogTask::State TRANSITION_TABLE
    [GyroConnectionWatchdogTask::GYRO_WATCHDOG_NUMBER_OF_STATES]
    [GyroConnectionWatchdogTask::GYRO_WATCHDOG_NUMBER_OF_EVENTS] =
  {
    {  // CRREATED
      GyroConnectionWatchdogTask::STARTING,  // RESET
      GyroConnectionWatchdogTask::EXPIRING,  // EXPIRED, should not happen
    },
    { // STARTING
      GyroConnectionWatchdogTask::RESETTING,  // RESET
      GyroConnectionWatchdogTask::EXPIRING,   // EXPIRED
    },
    {  // RESETTING
      GyroConnectionWatchdogTask::HAS_RESET,  // RESET
      GyroConnectionWatchdogTask::EXPIRING,   // EXPIRE
    },
    {  // HAS_RESET
      GyroConnectionWatchdogTask::HAS_RESET,  // RESET
      GyroConnectionWatchdogTask::EXPIRING,   // EXPIRE
    },
    {  //  EXPIRING
      GyroConnectionWatchdogTask::RESETTING,    // RESET
      GyroConnectionWatchdogTask::HAS_EXPIRED,  // EXPIRE
    },
    {  // HAS_EXPIRED
      GyroConnectionWatchdogTask::RESETTING,    // RESET
      GyroConnectionWatchdogTask::HAS_EXPIRED,  // EXPIRE
    },
  };


GyroConnectionWatchdogTask::GyroConnectionWatchdogTask(
    PullQueueHT<ConnectionStatusMessage>& connection_status_queue) :
      Task(
          "ESP32 Watchdog",
          STACK_DEPTH,
          PRIORITY),
          connection_status_queue_(connection_status_queue),
      state(CREATED),
      h_timer(NULL),
      timer_event_queue(10) {
}

GyroConnectionWatchdogTask::~GyroConnectionWatchdogTask(void) {
}

void GyroConnectionWatchdogTask::on_timer_expired(TimerHandle_t h_timer) {
  (static_cast<GyroConnectionWatchdogTask *>(pvTimerGetTimerID(h_timer)))
      ->expire();
}

void GyroConnectionWatchdogTask::expire(void) {
  timer_event_queue.send_message(&EXPIRE_MESSAGE, 0);
}

void GyroConnectionWatchdogTask::reset(void) {
  timer_event_queue.send_message(&RESET_MESSAGE, 0);
}

TaskHandle_t GyroConnectionWatchdogTask::start(void) {
  timer_event_queue.begin();
  h_timer = xTimerCreate(
      "Gyro Disconnect",
      1510,
      pdTRUE,
      this,
      on_timer_expired);
  TaskHandle_t h_task = create_and_start_task();
  Serial.println("Gyroscope connection task started.");
  return h_task;
}

void GyroConnectionWatchdogTask::task_loop(void) {
  EventMessage_t event_message;
  for (;;) {
    if (
        timer_event_queue.pull_message(&event_message)
        && event_message.event != Event::GYRO_WATCHDOG_NUMBER_OF_EVENTS) {
      state = TRANSITION_TABLE[state][event_message.event];
      switch (state) {
        case CREATED:
          // Assume connection down until shown otherwise.
          connection_status_queue_.send_message(&CONNECTION_DOWN, 0);
          break;
        case STARTING:
          xTimerStart(h_timer, 0);
          break;
        case RESETTING:
          xTimerReset(h_timer, 0);
          connection_status_queue_.send_message(&CONNECTION_UP, 0);
          break;
        case HAS_RESET:
          xTimerReset(h_timer, 0);
          break;
        case EXPIRING:
          connection_status_queue_.send_message(&CONNECTION_DOWN, 0);
          break;
        case HAS_EXPIRED:
          // Nothing to do
          break;
        case GYRO_WATCHDOG_NUMBER_OF_STATES:
          // Cannot happen
          break;
      }
    }
  }
}
