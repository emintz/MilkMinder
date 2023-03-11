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

#ifndef EVENTRELAYTASK_H_
#define EVENTRELAYTASK_H_

#include "Arduino.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#include "MotionNotificationMessage.h"
#include "Task.h"

class EventRelayTask :
    public Task {

  enum GyroscopeState {
    LID_IS_LEVEL,  // Top is level
    NEW_TILT_RECEIVED,  // New tilt signal received
    VERIFYING_TILT,  // Verifying motion, waiting for timeout
    TILT_CONFIRMED,  //  Tilt confirmed.
    TILT_SIGNAL_LOST,
    LAST_SEND_STATE,  // MUST be last
  };

  enum ReceiverConnectionState {
    UNKNOWN,
    CONNECTED,
    DISCONNECTED,
  };

private:

  enum State {
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
      [GYRO_NUMBER_OF_STATES][LAST_NOTIFICATION_STATUS];

  GyroscopeState tilt_state;
  State state;
  ReceiverConnectionState connection_state;
  QueueHandle_t h_tilt_notification_queue;
  QueueHandle_t h_send_to_receiver_queue;
  uint32_t tilt_start_time_millis;
  uint32_t tilt_signal_active_millis;
  TickType_t queue_wait_time_millis;
  MotionNotificationMessage notification_message;

  /**
   * The send task loop.
   */
  virtual void task_loop();

public:
  EventRelayTask();
  virtual ~EventRelayTask();

  /**
   * Initialize the send task.
   *
   * Parameters:
   *
   * Name                      Contents
   * ------------------------- ------------------------------------------------
   * h_tilt_notification_queue Incoming messages from the Gyroscope Task
   * h_send_to_receiver_queue  Messages to send via ESP NOW
   */
  void begin(
    QueueHandle_t h_tilt_notification_queue,
    QueueHandle_t h_send_to_receiver_queue);

  /**
   * Start the send loop
   */
  TaskHandle_t start_send_loop(void);
};

#endif /* EVENTRELAYTASK_H_ */
