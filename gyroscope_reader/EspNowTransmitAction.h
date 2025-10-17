/*
 * EspNowTransmitter.h
 *
 *  Created on: Dec 25, 2022
 *      Author: Eric Mintz
 *
 * Task that sends messages to an ESP-NOW receiver.
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

class EspNowTransmitAction : public TaskAction {
public:
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
   * Initialize ESP-NoW. Disable the error indication blink
   * and connect to the receiver. This might take some time.
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
  virtual void run(void);
};

#endif /* ESPNOWTRANSMITACTION_H_ */
