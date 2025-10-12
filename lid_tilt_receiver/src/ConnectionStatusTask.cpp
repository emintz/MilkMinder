/*
 * ConnectionStatusTask.cpp
 *
 *  Created on: Feb 20, 2023
 *      Author: Eric Mintz
 */

#include "ConnectionStatusTask.h"

const ConnectionStatusTask::State ConnectionStatusTask::TRANSITION_TABLE
    [NET_STATE_COUNT]
    [CONNECTION_STATUS_COUNT] = {
  { // NET_INITIALIZED
    NET_GOING_DOWN,  // CONNECTION_STATUS_DOWN
    NET_COMING_UP,  // CONNECTION_STATUS_UP
    NET_SENDER_PANIC,  // CONNECTION_STATUS_SENDER_FAILED
  },
  { // NET_GOING_DOWN
    NET_DISCONNECTED,  // CONNECTION_STATUS_DOWN
    NET_COMING_UP,  // CONNECTION_STATUS_UP
    NET_SENDER_PANIC,  // CONNECTION_STATUS_SENDER_FAILED
  },
  { // NET_DISCONNECTED
    NET_DISCONNECTED,  // CONNECTION_STATUS_DOWN
    NET_COMING_UP,  // CONNECTION_STATUS_UP
    NET_SENDER_PANIC,  // CONNECTION_STATUS_SENDER_FAILED
  },
  { // NET_GOING_UP
    NET_GOING_DOWN,  // CONNECTION_STATUS_DOWN
    NET_CONNECTED, // CONNECTION_STATUS_UP
    NET_SENDER_PANIC,  // CONNECTION_STATUS_SENDER_FAILED
  },
  { // NET_CONNECTED
    NET_GOING_DOWN,  // CONNECTION_STATUS_DOWN
    NET_CONNECTED,  // CONNECTION_STATUS_UP
    NET_SENDER_PANIC,  // CONNECTION_STATUS_SENDER_FAILED
  },
  { // NET_SENDER_PANIC
    NET_STATE_COUNT,
    NET_STATE_COUNT,
    NET_STATE_COUNT,
  }
};

ConnectionStatusTask::ConnectionStatusTask(
    DisconnectedLedTask *disconnected_led_task,
    uint8_t connected_led_pin,
    PullQueueHT<ConnectionStatusMessage>& connection_status_queue,
    PullQueueHT<DisplayMessage>& display_command_queue) :
        Task("Network status", 2048, 15),
        state(NET_INITIALIZED),
        disconnected_led_task(disconnected_led_task),
        connected_led_pin(connected_led_pin),
        connection_status_queue_(connection_status_queue),
        display_command_queue_(display_command_queue) {
}

ConnectionStatusTask::~ConnectionStatusTask() {
}

TaskHandle_t ConnectionStatusTask::start() {
  return create_and_start_task();
}

void ConnectionStatusTask::task_loop() {
  ConnectionStatusMessage connection_status_message;
  DisplayMessage display_command;
  for (;;) {
    if (connection_status_queue_.pull_message(&connection_status_message)) {
      if (connection_status_message.status != CONNECTION_STATUS_COUNT) {
        switch (state =
            TRANSITION_TABLE[state][connection_status_message.status]) {
        case NET_INITIALIZED:
          Serial.println("WIFI initializing.");
          break;
        case NET_GOING_DOWN:
          digitalWrite(connected_led_pin, LOW);
          disconnected_led_task->enable();
          display_command.command = LCD_DISCONNECTED;
          display_command_queue_.send_message(&display_command, 0);
          Serial.println("WIFI signal lost");
          break;
        case NET_DISCONNECTED:
          break;
        case NET_COMING_UP:
          Serial.println("WIFI connected.");
          disconnected_led_task->disable();
          digitalWrite(connected_led_pin, HIGH);
          display_command.command = LCD_CONNECTED;
          display_command_queue_.send_message(&display_command, 0);
          break;
        case NET_CONNECTED:
          break;
        case NET_SENDER_PANIC:
          disconnected_led_task->enable();
          digitalWrite(connected_led_pin, LOW);
          display_command.command = LCD_TRANSMITTER_PANIC;
          display_command_queue_.send_message(&display_command, 0);
          break;
        default:
          Serial.println("Default in connection status task.");
        }
      }
    }
  }
}
