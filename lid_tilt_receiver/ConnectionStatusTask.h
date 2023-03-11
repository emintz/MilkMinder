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
#include "freertos/queue.h"
#include "freertos/task.h"

#include "ConnectionStatus.h"
#include "DisconnectedLedTask.h"
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
  QueueHandle_t h_communication_event_queue;  // Provides incoming events
  QueueHandle_t h_display_command_queue;  // Outgoing display-related commands
  DisconnectedLedTask *disconnected_led_task;  // Blinks the disconnected LED
  uint8_t connected_led_pin;

public:
  ConnectionStatusTask(
      DisconnectedLedTask *disconnected_led_task,
      uint8_t connected_led_pin);
  virtual ~ConnectionStatusTask();

  TaskHandle_t start(
    QueueHandle_t h_connection_status_queue,
    QueueHandle_t h_display_message_queue);

  virtual void task_loop();
};

#endif /* CONNECTIONSTATUSTASK_H_ */
