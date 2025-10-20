/*
 * ReceiveAction.cpp
 *
 *  Created on: Feb 14, 2023
 *      Author: Eric Mintz
 */

#include "ReceiveAction.h"

#include "Arduino.h"

#include <stdlib.h>

#include "CommunicationEvent.h"
#include "DisplayMessage.h"
#include "PinAssignments.h"

/**
 * Carries notifications from the ESP-Now receive callback to the running
 * receive action.
 */
static PullQueueHT<CommunicationEvent> the_motion_notification_queue(3);

static uint8_t builtin_pin_state = LOW;

ReceiveAction::ReceiveAction(
    Resettable *watchdog_timer,
    PullQueueHT<LidPositionReport>& lid_position_report_queue) :
      watchdog_timer_(watchdog_timer),
      lid_position_report_queue_(lid_position_report_queue),
      lid_open_count_(0) {
}

ReceiveAction::~ReceiveAction() {
}

bool ReceiveAction::begin() {
  bool esp_now_status =
      ESP_OK == esp_now_init()
      && ESP_OK == esp_now_register_recv_cb(on_esp_now_received);
  Serial.println(esp_now_status
      ? "ESP_NOW initialized and ready to start."
      : "ESP_NOW initialization failed.");
  bool queue_status = the_motion_notification_queue.begin();
  Serial.println(queue_status
      ? "Motion notification queue ready"
      : "Motion notification queue startup FAILED.");

  return esp_now_status && queue_status;
}

void ReceiveAction::on_esp_now_received(
  const esp_now_recv_info* info,
  const uint8_t *received_data,
  int len) {
  MotionNotificationMessage *message =
    (MotionNotificationMessage*) received_data;
  CommunicationEvent comm_event;
  memset(&comm_event, 0, sizeof(comm_event));
  comm_event.event_type = *message;
  the_motion_notification_queue.send_message(&comm_event, pdMS_TO_TICKS(10));
}

void ReceiveAction::run() {
  CommunicationEvent event;
  memset(&event, 0, sizeof(event));
  LidPositionReport lid_position_report;

  memset(&lid_position_report, 0, sizeof(lid_position_report));

  for(;;) {
    memset(&event, 0, sizeof(event));
    if (the_motion_notification_queue.pull_message(&event)) {
      watchdog_timer_->reset();
      builtin_pin_state = (builtin_pin_state == LOW) ? HIGH : LOW;
      digitalWrite(BUILTIN_LED_PIN, builtin_pin_state);

      switch (event.event_type.status) {
        case LID_HAS_NOT_MOVED:
          lid_position_report.lid_position = LidPositionReport::LID_POS_CLOSED;
          lid_position_report_queue_.send_message(&lid_position_report, 0);
          break;
        case LID_RAISED:
          ++lid_open_count_;
          lid_position_report.lid_position =
              event.event_type.opened_count == lid_open_count_
                  ? LidPositionReport::LID_POS_OPEN
                  : LidPositionReport::LID_POS_SYNC_LOST;;
          lid_position_report_queue_.send_message(&lid_position_report, 0);
          break;
        case GYROSCOPE_SIGNAL_LOST:
          // TODO: support or remove. The transmitter does not send this
          //       at the moment.
          break;
        case PING:
          if (event.event_type.opened_count != lid_open_count_) {
            lid_position_report.lid_position =
                LidPositionReport::LID_POS_SYNC_LOST;
            lid_position_report_queue_.send_message(&lid_position_report, 0);
          }
          break;
        case LAST_NOTIFICATION_STATUS:  // Should not happen
          break;
      }
    }
  }
}

