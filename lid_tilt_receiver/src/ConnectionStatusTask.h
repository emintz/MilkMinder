/*
 * ConnectionStatusTask.h
 *
 *  Created on: Feb 20, 2023
 *      Author: Eric Mintz
 *
 * Indicates the connection status.
 */

#ifndef CONNECTIONSTATUSTASK_H_
#define CONNECTIONSTATUSTASK_H_

#include "Arduino.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "BlinkAction.h"

#include "CommunicationEvent.h"
#include "ConnectionStatus.h"
#include "ConnectionStatusTask.h"
#include "DisplayMessage.h"
#include "PullQueueHT.h"
#include "Task.h"

/**
 * A task that responds to connectivity events and indicates when the
 * network comes up and goes down, taking the following actions:
 *
 *   Illuminates the network connection indicator LED when the sender
 *   is connected and extinguishes it when the connection fails.
 *
 *   Directs the LCD task to show network connection status.
 *
 * The task implements a Moore-type finite state machine that transitions among
 * the states specified below in response to ConnectionStatus events.
 */
class ConnectionStatusTask :
  public Task {

  /**
   * FSM states
   */
  enum State {
    NET_INITIALIZED,
    NET_GOING_DOWN,
    NET_DISCONNECTED,
    NET_COMING_UP,
    NET_CONNECTED,
    NET_SENDER_PANIC,
    NET_STATE_COUNT,
  };

  static const State TRANSITION_TABLE[NET_STATE_COUNT][CONNECTION_STATUS_COUNT];

  State state;  // Machine state
  BlinkAction& disconnected_led_action_;
  uint8_t connected_led_pin_;
  PullQueueHT<ConnectionStatusMessage>& connection_status_queue_;
  PullQueueHT<DisplayMessage>& display_command_queue_;

public:
  ConnectionStatusTask(
      BlinkAction& disconnected_led_action,
      uint8_t connected_led_pin,
      PullQueueHT<ConnectionStatusMessage>& communications_event_queue,
      PullQueueHT<DisplayMessage>& display_command_queue);
  virtual ~ConnectionStatusTask();

  TaskHandle_t start();

  virtual void task_loop();
};

#endif /* CONNECTIONSTATUSTASK_H_ */
