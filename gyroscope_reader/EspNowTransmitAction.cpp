/*
 * EspNowTransmitter.cpp
 *
 *  Created on: Dec 25, 2022
 *      Author: Eric Mintz
 */

#include "EspNowTransmitter.h"

#include "TaskPriorities.h"

#include "PinAssignments.h"

enum EspSendState {
	SUCCESSFUL,  // Send succeeded
	FAILED,  // Send failed.
	ESP_SEND_STATUS_LAST,  // MUST be last
};

BlinkTask* EspNowTransmitter::global_blink_task = NULL;

void EspNowTransmitter::send_callback(
  const uint8_t *mac_address,
  esp_now_send_status_t send_status) {
  if (global_blink_task) {
    switch (send_status) {
    case ESP_NOW_SEND_SUCCESS:
      digitalWrite(GREEN_LED_PIN, HIGH);
      global_blink_task->suspend();
      break;
    case ESP_NOW_SEND_FAIL:
      digitalWrite(GREEN_LED_PIN, LOW);
      global_blink_task->resume();
      break;
    }
  } else {
    Serial.println("Blink task is unavailable.");
  }
}

const EspNowTransmitter::ConnectionState STATE_TRANSITION_TABLE
    [EspNowTransmitter::LAST_CONNECTION_STATE]
    [EspSendState::ESP_SEND_STATUS_LAST] = {
  {   // STARTING
    EspNowTransmitter::RECONNECTED,  // Successful
    EspNowTransmitter::STARTING,   // Failed
  },
  {	// RECONNECTED
    EspNowTransmitter::CONNECTED,  // Successful
    EspNowTransmitter::CONNECTION_LOST,  // Failed.
  },
  {	// CONNECTED
    EspNowTransmitter::CONNECTED,  // Successful
    EspNowTransmitter::CONNECTION_LOST,  // Failed.
  },
  {	// CONNECTION_LOST
    EspNowTransmitter::RECONNECTED,  // Successful
    EspNowTransmitter::DISCONNECTED,  // Failed
  },
  {	// DISCONNECTED
    EspNowTransmitter::RECONNECTED,
    EspNowTransmitter::DISCONNECTED,
  }
};

EspNowTransmitter::EspNowTransmitter(
    const uint8_t *peer_address,
      BlinkTask *blink_task) :
          Task(
              "ESP-Now transmitter",
              2048,
              ESP_NOW_SEND_PRIORITY),
      connection_state(STARTING),
      peer_address(peer_address),
      h_notification_send_queue(0),
      wait_for_incoming_in_ticks(pdMS_TO_TICKS(1)),
      builtin_led_state(LOW) {
  notification_message.status = PING;
  notification_message.temperature_celsius = ABSOLUTE_ZERO;
  start_time = millis();
}

EspNowTransmitter::~EspNowTransmitter() {
}

void EspNowTransmitter::task_loop() {
  bool send_message = false;
  for (;;) {
    BaseType_t receive_status = xQueueReceive(
        h_notification_send_queue,
        &notification_message,
        wait_for_incoming_in_ticks);
    if (receive_status != pdTRUE) {
      notification_message.status = GYROSCOPE_SIGNAL_LOST;
      notification_message.temperature_celsius = ABSOLUTE_ZERO;
    }
    esp_err_t send_status = esp_now_send(
      peer_address,
      (const uint8_t *)(&notification_message),
      sizeof(notification_message));
    EspSendState send_state =
      (send_status == ESP_OK) ? SUCCESSFUL : FAILED;
    builtin_led_state = builtin_led_state ? LOW : HIGH;
    digitalWrite(BUILTIN_LED_PIN, builtin_led_state);

    connection_state = STATE_TRANSITION_TABLE[connection_state][send_state];
    switch (connection_state) {
      case STARTING:
        send_message = true;
        break;
      case RECONNECTED:
        send_message = true;
        start_time = millis();
        global_blink_task->suspend();
        wait_for_incoming_in_ticks = pdMS_TO_TICKS(1000);
        break;
      case CONNECTED:
        if ((send_message = ((millis() - start_time) > 500))) {
          start_time = millis();
        }
        break;
      case CONNECTION_LOST:
        send_message = true;
        wait_for_incoming_in_ticks = pdMS_TO_TICKS(50);
        break;
      case DISCONNECTED:
        digitalWrite(GREEN_LED_PIN, LOW);
        if ((send_message = ((millis() - start_time) > 500))) {
          start_time = millis();
        }
        break;
      case LAST_CONNECTION_STATE:
        // Should never happen.
        break;
    }
  }
}

bool EspNowTransmitter::begin(QueueHandle_t h_notification_send_queue) {
  this->h_notification_send_queue = h_notification_send_queue;

  Serial.print("Initializing ESP-NOW ... ");
  esp_err_t esp_now_status = esp_now_init();
  Serial.println((esp_now_status == ESP_OK) ? "succeeded." : "failed.");

  Serial.print("Adding peer ... ");
  esp_now_peer_info peer_info;
  memset(&peer_info, 0, sizeof(peer_info));
  memcpy(peer_info.peer_addr, peer_address, ESP_NOW_ETH_ALEN);
  memset(peer_info.lmk, 0, ESP_NOW_KEY_LEN);
  peer_info.ifidx = WIFI_IF_STA;
  peer_info.encrypt = false;
  esp_err_t peer_add_status = esp_now_add_peer(&peer_info);
  if (peer_add_status == ESP_OK) {
    Serial.println("succeeded.");
  } else {
    Serial.print("failed with status: 0X");
    Serial.println(peer_add_status - ESP_ERR_ESPNOW_BASE);
  }
  bool callback_registration_status =
    esp_now_register_send_cb(send_callback) == ESP_OK;
  Serial.print("Registering send callback ... ");
  Serial.println(callback_registration_status ? "succeeded." : "failed.");

  return true;
}

TaskHandle_t EspNowTransmitter::start() {
    global_blink_task->resume();
  return create_and_start_task();
}
