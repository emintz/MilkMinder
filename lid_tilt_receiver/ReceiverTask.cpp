/*
 * ReceiverTask.cpp
 *
 *  Created on: Feb 14, 2023
 *      Author: Eric Mintz
 */

#include "Arduino.h"

#include "ReceiverTask.h"

#include "esp_now.h"

#include <stdlib.h>

#include "CommunicationEvent.h"
#include "DisplayMessage.h"
#include "LidPositionReport.h"
#include "PinAssignments.h"

static QueueHandle_t h_the_motion_notification_queue;

static uint8_t builtin_pin_state = LOW;

ReceiverTask::ReceiverTask(
    TimeTask *time_task,
    WatchdogTimer *watchdog_timer) :
      Task("Receiver", 2048, 4),
      h_communications_queue(NULL),
      h_lid_position_report_queue(NULL),
      watchdog_timer(watchdog_timer),
      time_task(time_task) {
}

ReceiverTask::~ReceiverTask() {
}

bool ReceiverTask::begin() {
  bool status = esp_now_init() == ESP_OK;
  if (!status) {
     Serial.println("ESP_NOW initialization failed.");
  } else {
    Serial.println("ESP_NOW initialized and ready to start.");
  }
  return status;
}

void ReceiverTask::on_esp_now_received(
  const uint8_t *mac,
  const uint8_t *received_data,
  int len) {
  MotionNotificationMessage *message =
    (MotionNotificationMessage*) received_data;
  CommunicationEvent comm_event;
  memset(&comm_event, 0, sizeof(comm_event));
  comm_event.event_type = *message;
  xQueueSendToBack(
      h_the_motion_notification_queue, &comm_event, pdMS_TO_TICKS(10));
}

void ReceiverTask::task_loop() {
  MotionNotificationMessage motion_notification_message;
  memset(&motion_notification_message, 0, sizeof(motion_notification_message));
  CommunicationEvent event;
  LidPositionReport lid_position_report;

  memset(&lid_position_report, 0, sizeof(lid_position_report));

  for(;;) {
    memset(&event, 0, sizeof(event));
    if (xQueueReceive(
        h_the_motion_notification_queue,
        &motion_notification_message,
        portMAX_DELAY) == pdTRUE) {
      uint8_t yellow = LOW;
      uint8_t green = LOW;
      watchdog_timer->reset();
      builtin_pin_state = (builtin_pin_state == LOW) ? HIGH : LOW;
      digitalWrite(BUILTIN_LED_PIN, builtin_pin_state);

      switch (motion_notification_message.status) {
        case LID_HAS_NOT_MOVED:
          lid_position_report.lid_position = LidPositionReport::LID_POS_CLOSED;
          xQueueSendToBack(
              h_lid_position_report_queue, &lid_position_report, 0);
          break;
        case LID_RAISED:
          lid_position_report.lid_position = LidPositionReport::LID_POS_OPEN;
          xQueueSendToBack(
              h_lid_position_report_queue, &lid_position_report, 0);
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

TaskHandle_t ReceiverTask::start(
    QueueHandle_t h_communications_event_queue,
    QueueHandle_t h_lid_position_report_queue) {
  h_the_motion_notification_queue =
      xQueueCreate(3, sizeof(MotionNotificationMessage));

  this->h_communications_queue = h_communications_event_queue;
//  this->h_delivery_event_queue = h_delivery_event_queue;
  this->h_lid_position_report_queue = h_lid_position_report_queue;

  if (!esp_now_register_recv_cb(on_esp_now_received) == ESP_OK) {
    Serial.println("Receive callback registration failed.");
  } else {
    Serial.println("ESP_NOW handler started.");
    // TODO: panic
  }

  return create_and_start_task();
}
