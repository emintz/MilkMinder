/*
 * GyroConnectionWatchdogAction.cpp
 *
 *  Created on: May 21, 2023
 *      Author: Eric Mintz
 */

#include <src/GyroConnectionWatchdogAction.h>


static ConnectionStatusMessage CONNECTION_DOWN = { CONNECTION_STATUS_DOWN };
static ConnectionStatusMessage CONNECTION_UP = { CONNECTION_STATUS_UP };

GyroConnectionWatchdogAction::EventMessage_t
    GyroConnectionWatchdogAction::EXPIRE_MESSAGE = {
      GyroConnectionWatchdogAction::EXPIRE,
};
GyroConnectionWatchdogAction::EventMessage_t
    GyroConnectionWatchdogAction::RESET_MESSAGE = {
      GyroConnectionWatchdogAction::RESET,
};

static const GyroConnectionWatchdogAction::State TRANSITION_TABLE
    [GyroConnectionWatchdogAction::GYRO_WATCHDOG_NUMBER_OF_STATES]
    [GyroConnectionWatchdogAction::GYRO_WATCHDOG_NUMBER_OF_EVENTS] =
  {
    {  // CRREATED
      GyroConnectionWatchdogAction::STARTING,  // RESET
      GyroConnectionWatchdogAction::EXPIRING,  // EXPIRED, should not happen
    },
    { // STARTING
      GyroConnectionWatchdogAction::RESETTING,  // RESET
      GyroConnectionWatchdogAction::EXPIRING,   // EXPIRED
    },
    {  // RESETTING
      GyroConnectionWatchdogAction::HAS_RESET,  // RESET
      GyroConnectionWatchdogAction::EXPIRING,   // EXPIRE
    },
    {  // HAS_RESET
      GyroConnectionWatchdogAction::HAS_RESET,  // RESET
      GyroConnectionWatchdogAction::EXPIRING,   // EXPIRE
    },
    {  //  EXPIRING
      GyroConnectionWatchdogAction::RESETTING,    // RESET
      GyroConnectionWatchdogAction::HAS_EXPIRED,  // EXPIRE
    },
    {  // HAS_EXPIRED
      GyroConnectionWatchdogAction::RESETTING,    // RESET
      GyroConnectionWatchdogAction::HAS_EXPIRED,  // EXPIRE
    },
  };


GyroConnectionWatchdogAction::GyroConnectionWatchdogAction(
    PullQueueHT<ConnectionStatusMessage>& connection_status_queue) :
      connection_status_queue_(connection_status_queue),
      expiration_(*this),
      watchdog_timer_("Gyro Watchdog", expiration_, 1510),
      state_(CREATED),
      timer_event_queue_(10) {
}

GyroConnectionWatchdogAction::~GyroConnectionWatchdogAction(void) {
}

void GyroConnectionWatchdogAction::expire(void) {
  timer_event_queue_.send_message(&EXPIRE_MESSAGE, 0);
}

void GyroConnectionWatchdogAction::reset(void) {
  timer_event_queue_.send_message(&RESET_MESSAGE, 0);
}

void GyroConnectionWatchdogAction::run(void) {
  EventMessage_t event_message;
  timer_event_queue_.begin();
  for (;;) {
    if (
        timer_event_queue_.pull_message(&event_message)
        && event_message.event != Event::GYRO_WATCHDOG_NUMBER_OF_EVENTS) {
      state_ = TRANSITION_TABLE[state_][event_message.event];
      switch (state_) {
        case CREATED:
          // Assume connection down until shown otherwise.
          connection_status_queue_.send_message(&CONNECTION_DOWN, 0);
          break;
        case STARTING:
          watchdog_timer_.begin();
          break;
        case RESETTING:
          watchdog_timer_.reset();
          connection_status_queue_.send_message(&CONNECTION_UP, 0);
          break;
        case HAS_RESET:
          watchdog_timer_.reset();
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
