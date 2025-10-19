/*
 * SendTask.cpp
 *
 *  Created on: Dec 24, 2022
 *      Author: Eric Mintz
 *
 * Monitors the tilt message queue and notifies the receiver when the
 * lid is tilted.
 */

#include <EventRelayAction.h>

#include "Arduino.h"

#include "TaskPriorities.h"

const EventRelayAction::State EventRelayAction::TRANSITION_TABLE
    [static_cast<size_t>(EventRelayAction::State::GYRO_NUMBER_OF_STATES)]
    [LAST_NOTIFICATION_STATUS] =
{
  {  // EventRelayAction::State::GYRO_CREATED
      EventRelayAction::State::GYRO_NEW_CLOSURE_RECEIVED, // LID_HAS_NOT_MOVED
      EventRelayAction::State::GYRO_NEW_OPEN_RECEIVED, // LID_RAISED
      EventRelayAction::State::GYRO_SIGNAL_LOST, // GYROSCOPE_SIGNAL_LOST
      EventRelayAction::State::GYRO_NUMBER_OF_STATES, // PING
  },
  {  // EventRelayAction::State::GYRO_NEW_CLOSURE_RECEIVED
      EventRelayAction::State::GYRO_VERIFYING_CLOSURE, // LID_HAS_NOT_MOVED
      EventRelayAction::State::GYRO_VERIFYING_OPEN, // LID_RAISED
      EventRelayAction::State::GYRO_SIGNAL_LOST, // GYROSCOPE_SIGNAL_LOST
      EventRelayAction::State::GYRO_VERIFYING_CLOSURE, // PING
  },
  {  // EventRelayAction::State::GYRO_VERIFYING_CLOSURE
      EventRelayAction::State::GYRO_VERIFYING_CLOSURE, // LID_HAS_NOT_MOVED
      EventRelayAction::State::GYRO_NEW_OPEN_RECEIVED, // LID_RAISED
      EventRelayAction::State::GYRO_SIGNAL_LOST, // GYROSCOPE_SIGNAL_LOST
      EventRelayAction::State::GYRO_VERIFYING_CLOSURE, // PING
  },
  {  // EventRelayAction::State::GYRO_CONFIRMED_CLOSURE
      EventRelayAction::State::GYRO_NUMBER_OF_STATES, // LID_HAS_NOT_MOVED -- ignored
      EventRelayAction::State::GYRO_NEW_OPEN_RECEIVED, // LID_RAISED
      EventRelayAction::State::GYRO_SIGNAL_LOST, // GYROSCOPE_SIGNAL_LOST
      EventRelayAction::State::GYRO_NUMBER_OF_STATES, // PING
  },
  {  // EventRelayAction::State::GYRO_NEW_OPEN_RECEIVED
      EventRelayAction::State::GYRO_NEW_CLOSURE_RECEIVED, // LID_HAS_NOT_MOVED
      EventRelayAction::State::GYRO_VERIFYING_OPEN, // LID_RAISED
      EventRelayAction::State::GYRO_SIGNAL_LOST, // GYROSCOPE_SIGNAL_LOST
      EventRelayAction::State::GYRO_VERIFYING_OPEN, // PING
  },
  {  // EventRelayAction::State::GYRO_VERIFYING_OPEN
      EventRelayAction::State::GYRO_NEW_CLOSURE_RECEIVED, // LID_HAS_NOT_MOVED
      EventRelayAction::State::GYRO_VERIFYING_OPEN, // LID_RAISED
      EventRelayAction::State::GYRO_SIGNAL_LOST, // GYROSCOPE_SIGNAL_LOST
      EventRelayAction::State::GYRO_VERIFYING_OPEN, // PING
  },
  {  // EventRelayAction::State::GYRO_CONFIRMED_OPEN
      EventRelayAction::State::GYRO_NEW_CLOSURE_RECEIVED, // LID_HAS_NOT_MOVED
      EventRelayAction::State::GYRO_CONFIRMED_OPEN, // LID_RAISED
      EventRelayAction::State::GYRO_SIGNAL_LOST, // GYROSCOPE_SIGNAL_LOST
      EventRelayAction::State::GYRO_NUMBER_OF_STATES, // PING
  },
  {  // EventRelayAction::State::GYRO_SIGNAL_LOST
      EventRelayAction::State::GYRO_NEW_CLOSURE_RECEIVED, // LID_HAS_NOT_MOVED
      EventRelayAction::State::GYRO_NEW_OPEN_RECEIVED, // LID_RAISED
      EventRelayAction::State::GYRO_NUMBER_OF_STATES, // GYROSCOPE_SIGNAL_LOST
      EventRelayAction::State::GYRO_NUMBER_OF_STATES, // PING
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
#define CONNECTED_QUEUE_WAIT_MILLIS 1000
#define CONNECTED_QUEUE_WAIT_TICKS pdMS_TO_TICKS(CONNECTED_QUEUE_WAIT_MILLIS)

/**
 * Wait time for message receipt when the receiver is disconnected
 */
#define DISCONNECTED_QUEUE_WAIT_MILLIS ()
#define DISCONNECTED_QUEUE_WAIT_TICKS pdMS_TO_TICKS(DISCONNECTED_QUEUE_WAIT_MILLIS)

EventRelayAction::EventRelayAction(
    PullQueueHT<MotionNotificationMessage>& tilt_notification_queue,
    PullQueueHT<MotionNotificationMessage>& send_to_receiver_queue) :
        tilt_notification_queue_(tilt_notification_queue),
        send_to_receiver_queue_(send_to_receiver_queue),
        state(EventRelayAction::State::GYRO_CREATED),
        connection_state_(EventRelayAction::ReceiverConnectionState::UNKNOWN) {
  notification_message_.status = LID_HAS_NOT_MOVED;
  notification_message_.temperature_celsius = ABSOLUTE_ZERO;
}

EventRelayAction::~EventRelayAction() {
}

void EventRelayAction::run() {
  Serial.println("Event relay task started.");
  MotionNotificationMessage message;
  State maybe_next_state;
  MotionStatus motion_status;
  uint32_t lid_moved_at_milliseconds = 0;
  uint32_t lid_first_opened_at_milliseconds = 0;
  uint16_t number_of_times_lid_opened = 0;
  Serial.print("Initial state: ");
  Serial.println(static_cast<size_t>(state));
  for (;;) {
    motion_status = PING;
    if (
        tilt_notification_queue_.pull_message(
            &message, CONNECTED_QUEUE_WAIT_MILLIS)
        && message.status != LAST_NOTIFICATION_STATUS) {
      maybe_next_state = TRANSITION_TABLE[static_cast<size_t>(state)][message.status];
      if (maybe_next_state != EventRelayAction::State::GYRO_NUMBER_OF_STATES) {
        switch (state = maybe_next_state) {
        case EventRelayAction::State::GYRO_CREATED:
          // Should never happen
          break;
        case EventRelayAction::State::GYRO_NEW_CLOSURE_RECEIVED:
          lid_moved_at_milliseconds = millis();
          break;
        case EventRelayAction::State::GYRO_VERIFYING_CLOSURE:
          if (CONFIRMATION_TIME_MS <=  millis() - lid_moved_at_milliseconds) {
            state = EventRelayAction::State::GYRO_CONFIRMED_CLOSURE;
            motion_status = LID_HAS_NOT_MOVED;
          }
          break;
        case EventRelayAction::State::GYRO_CONFIRMED_CLOSURE:
          break;
        case EventRelayAction::State::GYRO_NEW_OPEN_RECEIVED:
          lid_moved_at_milliseconds = millis();
          break;
        case EventRelayAction::State::GYRO_VERIFYING_OPEN:
          if (CONFIRMATION_TIME_MS <= millis() - lid_moved_at_milliseconds) {
            state = EventRelayAction::State::GYRO_CONFIRMED_OPEN;
            motion_status = LID_RAISED;
            if (0 == number_of_times_lid_opened) {
              lid_first_opened_at_milliseconds = millis();
            }
            ++number_of_times_lid_opened;
            Serial.printf(
                "Lid opened %u time(s). First open was %lu microseconds ago.\n",
                number_of_times_lid_opened,
                millis() - lid_first_opened_at_milliseconds);
          }
          break;
        case EventRelayAction::State::GYRO_CONFIRMED_OPEN:
          break;
        case EventRelayAction::State::GYRO_SIGNAL_LOST:
          break;
        case EventRelayAction::State::GYRO_NUMBER_OF_STATES:
          break;
        }
      }
      memset(&message, 0, sizeof(message));
      message.status = motion_status;
      if (0 < number_of_times_lid_opened) {
        message.when_opened = millis() - lid_first_opened_at_milliseconds;
      }
      message.opened_count = number_of_times_lid_opened;
      send_to_receiver_queue_.send_message(&message, 10);
    }
  }
}
