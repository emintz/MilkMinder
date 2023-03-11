/*
 * SendTask.cpp
 *
 *  Created on: Dec 24, 2022
 *      Author: Eric Mintz
 *
 * Monitors the tilt message queue and notifies the receiver when the
 * lid is tilted.
 */

#include "EventRelayTask.h"

#include "Arduino.h"

#include "TaskPriorities.h"

const EventRelayTask::State EventRelayTask::TRANSITION_TABLE
    [GYRO_NUMBER_OF_STATES][LAST_NOTIFICATION_STATUS] =
{
  {  // GYRO_CREATED
      GYRO_NEW_CLOSURE_RECEIVED, // LID_HAS_NOT_MOVED
      GYRO_NEW_OPEN_RECEIVED, // LID_RAISED
      GYRO_SIGNAL_LOST, // GYROSCOPE_SIGNAL_LOST
      GYRO_NUMBER_OF_STATES, // PING
  },
  {  // GYRO_NEW_CLOSURE_RECEIVED
      GYRO_VERIFYING_CLOSURE, // LID_HAS_NOT_MOVED
      GYRO_VERIFYING_OPEN, // LID_RAISED
      GYRO_SIGNAL_LOST, // GYROSCOPE_SIGNAL_LOST
      GYRO_VERIFYING_CLOSURE, // PING
  },
  {  // GYRO_VERIFYING_CLOSURE
      GYRO_VERIFYING_CLOSURE, // LID_HAS_NOT_MOVED
      GYRO_NEW_OPEN_RECEIVED, // LID_RAISED
      GYRO_SIGNAL_LOST, // GYROSCOPE_SIGNAL_LOST
      GYRO_VERIFYING_CLOSURE, // PING
  },
  {  // GYRO_CONFIRMED_CLOSURE
      GYRO_NUMBER_OF_STATES, // LID_HAS_NOT_MOVED -- ignored
      GYRO_NEW_OPEN_RECEIVED, // LID_RAISED
      GYRO_SIGNAL_LOST, // GYROSCOPE_SIGNAL_LOST
      GYRO_NUMBER_OF_STATES, // PING
  },
  {  // GYRO_NEW_OPEN_RECEIVED
      GYRO_NEW_CLOSURE_RECEIVED, // LID_HAS_NOT_MOVED
      GYRO_VERIFYING_OPEN, // LID_RAISED
      GYRO_SIGNAL_LOST, // GYROSCOPE_SIGNAL_LOST
      GYRO_VERIFYING_OPEN, // PING
  },
  {  // GYRO_VERIFYING_OPEN
      GYRO_NEW_CLOSURE_RECEIVED, // LID_HAS_NOT_MOVED
      GYRO_VERIFYING_OPEN, // LID_RAISED
      GYRO_SIGNAL_LOST, // GYROSCOPE_SIGNAL_LOST
      GYRO_VERIFYING_OPEN, // PING
  },
  {  // GYRO_CONFIRMED_OPEN
      GYRO_NEW_CLOSURE_RECEIVED, // LID_HAS_NOT_MOVED
      GYRO_VERIFYING_OPEN, // LID_RAISED
      GYRO_SIGNAL_LOST, // GYROSCOPE_SIGNAL_LOST
      GYRO_NUMBER_OF_STATES, // PING
  },
  {  // GYRO_SIGNAL_LOST
      GYRO_NEW_CLOSURE_RECEIVED, // LID_HAS_NOT_MOVED
      GYRO_NEW_OPEN_RECEIVED, // LID_RAISED
      GYRO_NUMBER_OF_STATES, // GYROSCOPE_SIGNAL_LOST
      GYRO_NUMBER_OF_STATES, // PING
  },
};

/**
 * Wait time for tilt confirmation. The task notifies the receiver when
 * the the gyroscope task indicates tilt for the specified time. Note
 * that the time is in milliseconds.
 */
#define CONFIRMATION_TIME_MS 2500

/**
 * Wait time for message receipt when the receiver is connected
 */
#define CONNECTED_QUEUE_WAIT_MILLIS pdMS_TO_TICKS(1000)

/**
 * Wait time for message receipt when the receiver is disconnected
 */
#define DISCONNECTED_QUEUE_WAIT_MILLIS pdMS_TO_TICKS(10)

EventRelayTask::EventRelayTask() :
    Task(
        "Gyroscope event relay task",
        2048,
        ESP_NOW_SEND_PRIORITY),
    tilt_state(LID_IS_LEVEL),
    state(GYRO_CREATED),
    connection_state(UNKNOWN),
    h_tilt_notification_queue(0),
    h_send_to_receiver_queue(0),
    tilt_start_time_millis(0),
    tilt_signal_active_millis(0),
    queue_wait_time_millis(CONNECTED_QUEUE_WAIT_MILLIS) {
  notification_message.status = LID_HAS_NOT_MOVED;
  notification_message.temperature_celsius = ABSOLUTE_ZERO;
}

EventRelayTask::~EventRelayTask() {
}

void EventRelayTask::begin(
    QueueHandle_t h_tilt_notification_queue,
    QueueHandle_t h_send_to_receiver_queue) {
  this->h_tilt_notification_queue = h_tilt_notification_queue;
  this->h_send_to_receiver_queue = h_send_to_receiver_queue;
}

void EventRelayTask::task_loop() {
  MotionNotificationMessage message;
  State maybe_next_state;
  uint32_t lid_moved_at_milliseconds = 0;
  for (;;) {
    if (xQueueReceive(
        h_tilt_notification_queue,
        &message,
        CONNECTED_QUEUE_WAIT_MILLIS)
        && message.status != LAST_NOTIFICATION_STATUS) {
      maybe_next_state = TRANSITION_TABLE[state][message.status];
      if (maybe_next_state != GYRO_NUMBER_OF_STATES) {
        switch (state = maybe_next_state) {
        case GYRO_CREATED:
          // Should never happen
          break;
        case GYRO_NEW_CLOSURE_RECEIVED:
          lid_moved_at_milliseconds = millis();
          message.status = PING;
          break;
        case GYRO_VERIFYING_CLOSURE:
          if (CONFIRMATION_TIME_MS <=  millis() - lid_moved_at_milliseconds) {
            state = GYRO_CONFIRMED_CLOSURE;
          } else {
            message.status = PING;
          }
          break;
        case GYRO_CONFIRMED_CLOSURE:
          break;
        case GYRO_NEW_OPEN_RECEIVED:
          lid_moved_at_milliseconds = millis();
          break;
        case GYRO_VERIFYING_OPEN:
          if (CONFIRMATION_TIME_MS <= millis() - lid_moved_at_milliseconds) {
            state = GYRO_CONFIRMED_OPEN;
          } else {
            message.status = PING;
          }
          break;
        case GYRO_CONFIRMED_OPEN:
          break;
        case GYRO_SIGNAL_LOST:
          break;
        case GYRO_NUMBER_OF_STATES:
          break;
        }
      }
      xQueueSendToBack(
        h_send_to_receiver_queue,
        &message,
        pdMS_TO_TICKS(10));
    }
  }
}

TaskHandle_t EventRelayTask::start_send_loop() {
  return create_and_start_task();
}
