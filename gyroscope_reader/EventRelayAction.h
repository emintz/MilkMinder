/*
 * SendTask.h
 *
 *  Created on: Dec 24, 2022
 *      Author: Eric Mintz
 *
 * Monitors the motion notification queue and alerts the receiver when
 * the lid is tilted. The sender monitors Z acceleration to determine
 * tilt. It relays incoming messages from the Gyroscope Task to the
 * ESP NOW transmitter.
 */

#ifndef EVENTRELAYACTION_H_
#define EVENTRELAYACTION_H_

#include "Arduino.h"
#include "freertos/FreeRTOS.h"

#include "PullQueueHT.h"

#include "MotionNotificationMessage.h"
#include "TaskAction.h"

class EventRelayAction : public TaskAction {

  enum class ReceiverConnectionState {
    UNKNOWN,
    CONNECTED,
    DISCONNECTED,
  };

private:

  enum class State {
    GYRO_CREATED,
    GYRO_NEW_CLOSURE_RECEIVED,
    GYRO_VERIFYING_CLOSURE,
    GYRO_CONFIRMED_CLOSURE,
    GYRO_NEW_OPEN_RECEIVED,
    GYRO_VERIFYING_OPEN,
    GYRO_CONFIRMED_OPEN,
    GYRO_SIGNAL_LOST,
    GYRO_NUMBER_OF_STATES,  // MUST be last.
  };

  static const State TRANSITION_TABLE
      [static_cast<size_t>(State::GYRO_NUMBER_OF_STATES)]
      [LAST_NOTIFICATION_STATUS];

  PullQueueHT<MotionNotificationMessage>& tilt_notification_queue_;
  PullQueueHT<MotionNotificationMessage>& send_to_receiver_queue_;
  State state;
  ReceiverConnectionState connection_state;
  MotionNotificationMessage notification_message;

public:
  EventRelayAction(
      PullQueueHT<MotionNotificationMessage>& tilt_notification_queue,
      PullQueueHT<MotionNotificationMessage>& send_to_receiver_queue);
  virtual ~EventRelayAction();

  /**
   * The action loop.
   */
  virtual void run() override;
};

#endif /* EVENTRELAYACTION_H_ */
