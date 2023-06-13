/*
 * GyroConnectionWatchdogTask.cpp
 *
 *  Created on: May 21, 2023
 *      Author: Eric Mintz
 */

#include "GyroConnectionWatchdogTask.h"

#include "ConnectionStatus.h"

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
    BlinkTask *blink_task) :
        Task(
            "ESP32 Watchdog",
            STACK_DEPTH,
            PRIORITY),
      blink_task(blink_task),
      state(CREATED),
      h_timer(NULL),
      h_connection_status_queue(NULL),
      h_timer_event_queue(NULL) {
}

GyroConnectionWatchdogTask::~GyroConnectionWatchdogTask(void) {
}

void GyroConnectionWatchdogTask::on_timer_expired(TimerHandle_t h_timer) {
  (static_cast<GyroConnectionWatchdogTask *>(pvTimerGetTimerID(h_timer)))
      ->expire();
}

void GyroConnectionWatchdogTask::expire(void) {
  xQueueSendToBack(h_timer_event_queue, &EXPIRE_MESSAGE, 0);
}

void GyroConnectionWatchdogTask::reset(void) {
  xQueueSendToBack(h_timer_event_queue, &RESET_MESSAGE, 0);
}

TaskHandle_t GyroConnectionWatchdogTask::start(
    QueueHandle_t h_communications_event_queue) {
  this->h_connection_status_queue = h_communications_event_queue;
  h_timer_event_queue = xQueueCreate(sizeof(EventMessage_t), 10);
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
        xQueueReceive(h_timer_event_queue, &event_message, portMAX_DELAY)
            == pdPASS
        && event_message.event != GYRO_WATCHDOG_NUMBER_OF_STATES) {
      state = TRANSITION_TABLE[state][event_message.event];
      switch (state) {
        case CREATED:
          // Assume connection down until shown otherwise.
          xQueueSendToBack(
              h_connection_status_queue,
              &CONNECTION_DOWN,
              0);
          break;
        case STARTING:
          xTimerStart(h_timer, 0);
          break;
        case RESETTING:
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
          xQueueSendToBack(
              h_connection_status_queue,
              &CONNECTION_DOWN,
              0);
          break;
        case HAS_EXPIRED:
          // Nothing to do
          break;
      }
    }
  }
}
