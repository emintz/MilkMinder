/*
 * ReceiverTask.cpp
 *
 *  Created on: Feb 14, 2023
 *      Author: Eric Mintz
 */

#include "Arduino.h"

#include "ReceiverTask.h"

#include <stdlib.h>

#include "CommunicationEvent.h"
#include "DisplayMessage.h"
#include "PinAssignments.h"

static PullQueueHT<CommunicationEvent> the_motion_notification_queue(3);

static uint8_t builtin_pin_state = LOW;

ReceiverTask::ReceiverTask(
    TimeTask *time_task,
    Resettable *watchdog_timer,
    PullQueueHT<LidPositionReport>& lid_position_report_queue) :
      Task("Receiver", 2048, 4),
      time_task(time_task),
      watchdog_timer(watchdog_timer),
      lid_position_report_queue_(lid_position_report_queue) {
}

ReceiverTask::~ReceiverTask() {
}

bool ReceiverTask::begin() {
  bool esp_now_status = esp_now_init() == ESP_OK;
  Serial.println(esp_now_status
      ? "ESP_NOW initialized and ready to start."
      : "ESP_NOW initialization failed.");
  bool queue_status = the_motion_notification_queue.begin();
  Serial.println(queue_status
      ? "Motion notification queue ready"
      : "Motion notification queue startup FAILED.");

  return esp_now_status && queue_status;
}

void ReceiverTask::on_esp_now_received(
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

void ReceiverTask::task_loop() {
  CommunicationEvent event;
  memset(&event, 0, sizeof(event));
  LidPositionReport lid_position_report;

  memset(&lid_position_report, 0, sizeof(lid_position_report));

  for(;;) {
    memset(&event, 0, sizeof(event));
    if (the_motion_notification_queue.pull_message(&event)) {
      watchdog_timer->reset();
      builtin_pin_state = (builtin_pin_state == LOW) ? HIGH : LOW;
      digitalWrite(BUILTIN_LED_PIN, builtin_pin_state);

      switch (event.event_type.status) {
        case LID_HAS_NOT_MOVED:
          lid_position_report.lid_position = LidPositionReport::LID_POS_CLOSED;
          lid_position_report_queue_.send_message(&lid_position_report, 0);
          break;
        case LID_RAISED:
          lid_position_report.lid_position = LidPositionReport::LID_POS_OPEN;
          lid_position_report_queue_.send_message(&lid_position_report, 0);
          break;
        case GYROSCOPE_SIGNAL_LOST:
          // TODO: support or remove. The transmitter does not send this
          //       at the moment.
          break;
        case PING:
          break;
        case LAST_NOTIFICATION_STATUS:  // Should not happen
          break;
      }
    }
  }
}

TaskHandle_t ReceiverTask::start() {
  if (!esp_now_register_recv_cb(on_esp_now_received) == ESP_OK) {
    Serial.println("Receive callback registration failed.");
    // TODO: panic
 } else {
    Serial.println("ESP_NOW handler started.");
 }

  return create_and_start_task();
}
