/*
 * EspNowTransmitter.h
 *
 *  Created on: Dec 25, 2022
 *      Author: Eric Mintz
 *
 * Task that sends messages to an ESP-NOW receiver and manages
 * connection indication.
 *
 * When ESP-Now is working properly, the green LED is illuminated and
 * the red LED is dark. When the ESP-Now connection fails, the green
 * LED is dark and the red LED blinks.
 *
 * ESP-Now documentation resides here:
 * https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/network/esp_now.html
 *
 */

#ifndef ESPNOWTRANSMITACTION_H_
#define ESPNOWTRANSMITACTION_H_

#include "Arduino.h"

#include "esp_now.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "PullQueueHT.h"
#include "TaskAction.h"

#include "MotionNotificationMessage.h"

/**
 * Sends notifications to the lid tilt receiver. The class provides two
 * components:
 *
 * 1. The transmitter that receives debounced lid movement events
 * 2. A callback that monitors the connection.
 *
 *
 */
class EspNowTransmitAction : public TaskAction {
public:
  /**
   * State of the connection. The state controls LED settings that
   * MUST change on only when the ESP-Now connection status changes.
   */
  enum ConnectionState {
    STARTING,  // Establishing connection at startup.
    RECONNECTED,  // Connection re-esablished after outage,
    CONNECTED,  //  The receiver is connected and receiving messages.
    CONNECTION_LOST,  // The connection has been dropped.
    DISCONNECTED,  // The receiver is not receiving messages.
    LAST_CONNECTION_STATE,  // MUST be last
  };

private:
  PullQueueHT<MotionNotificationMessage>& lid_position_queue_;
  ConnectionState connection_state;
  const uint8_t *peer_address;
  uint32_t start_time;
  MotionNotificationMessage notification_message;
  TickType_t wait_for_incoming_in_ticks;
  uint8_t builtin_led_state;

  /**
   * ESP-Now send callback. The espnow_start() registers this method
   * as a send callback.
   *
   * Arguments
   *
   * Name                      Contents
   * ------------------------- ------------------------------------------------
   * mac_address               The receiving MAC address
   * send_status               Send status indicating success or failure
   */
  static void send_callback(
    const uint8_t *mac_address,
    esp_now_send_status_t send_status);

public:

  static bool begin(void);

  /**
   * Constructor.
   *
   * Arguments
   *
   * Name                      Contents
   * ------------------------- ------------------------------------------------
   * peer_address              The receiver's MAC address consisting of 6
   *                           unsigned bytes. See CommunicationSettings.h
   * lid_position_queue        Provides lid position notifications
   */

  EspNowTransmitAction(
    const uint8_t *peer_address,
    PullQueueHT<MotionNotificationMessage>& lid_position_queue);
  virtual ~EspNowTransmitAction();

  /**
   * Initializes ESP-NoW. Initializes ESP-Now, adds the receiver as a pear,
   * and registers the send callback send_callback. Disables the error
   * indication blink and connect to the receiver. Connecting might take
   * significant time.
   *
   * Note: BE SURE to invoke EspNowTransmitter::espnow_start() before sending
   * messages on the notification send queue. Sending messages before
   * invoking begin() will cause undefined (and probably undesired)
   * behavior.
   *
   * TODO: consider implementing a timeout.
   */
  bool espnow_start();

  /**
   * Invoked by the containing task to run the action. Application code
   * MUST NOT invoke.
   */
  virtual void run(void) override;
};

#endif /* ESPNOWTRANSMITACTION_H_ */
