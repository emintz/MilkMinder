/*
 * DeliveryStatusTask.cpp
 *
 *  Created on: Feb 23, 2023
 *      Author: eric
 */

#include "DeliveryStatusTask.h"

#include "AlarmTask.h"
#include "DeliveryLEDIlluminationStatus.h"
#include "DisplayMessage.h"
#include "PinAssignments.h"

static const AlarmTask::AlarmTaskMessage CONNECTED_ALARM =
    { AlarmTask::ALARM_EVENT_CONNECTED
};
static const AlarmTask::AlarmTaskMessage DELIVERED_ALARM = {
    AlarmTask::ALARM_EVENT_DELIVERED
};
static const AlarmTask::AlarmTaskMessage LID_OPEN_ALARM = {
    AlarmTask::ALARM_EVENT_LID_OPEN
};
static const AlarmTask::AlarmTaskMessage TRANSMITTER_PANIC_ALARM = {
    AlarmTask::ALARM_EVENT_TRANSMITTER_PANIC
};

static const LedIlluminationMessage LED_OFF = { DELIVERY_LED_OFF };
static const LedIlluminationMessage LED_BLINK = { DELIVERY_LED_BLINK };
static const LedIlluminationMessage LED_ON = { DELIVERY_LED_ON };

const DeliveryStatus DeliveryStatusTask::TRANSITION_TABLE
    [DELIVERY_STATUS_COUNT][DELIVERY_EVENT_COUNT] =
{
  { // DELIVERY_STATUS_CREATED
    DELIVERY_STATUS_STARTING, // DELIVERY_EVENT_STARTING
    DELIVERY_STATUS_SYSTEM_FAILURE, // DELIVERY_EVENT_LISTENING
    DELIVERY_STATUS_SYSTEM_FAILURE, // DELIVERY_EVENT_LID_OPENED
    DELIVERY_STATUS_SYSTEM_FAILURE, // DELIVERY_EVENT_LID_CLOSED
    DELIVERY_STATUS_SYSTEM_FAILURE, // DELIVERY_EVENT_TRANSMITTER_FAILED
    DELIVERY_STATUS_SYSTEM_FAILURE, // DELIVERY_STATUS_INTERNAL_ERROR
  },
  { // DELIVERY_STATUS_STARTING
    DELIVERY_STATUS_SYSTEM_FAILURE, // DELIVERY_EVENT_STARTING
    DELIVERY_STATUS_AWAITING, // DELIVERY_EVENT_LISTENING
    DELIVERY_STATUS_SYSTEM_FAILURE, // DELIVERY_EVENT_LID_OPENED
    DELIVERY_STATUS_SYSTEM_FAILURE, // DELIVERY_EVENT_LID_CLOSED
    DELIVERY_STATUS_SYSTEM_FAILURE, // DELIVERY_EVENT_TRANSMITTER_FAILED
    DELIVERY_STATUS_SYSTEM_FAILURE, // DELIVERY_STATUS_INTERNAL_ERROR
  },
  { // DELIVERY_STATUS_AWAITING
    DELIVERY_STATUS_SYSTEM_FAILURE, // DELIVERY_EVENT_STARTING
    DELIVERY_STATUS_SYSTEM_FAILURE, // DELIVERY_EVENT_LISTENING
    DELIVERY_STATUS_IN_PROGRESS, // DELIVERY_EVENT_LID_OPENED
    DELIVERY_STATUS_COUNT, // DELIVERY_EVENT_LID_CLOSED
    DELIVERY_STATUS_SYSTEM_FAILURE, // DELIVERY_EVENT_TRANSMITTER_FAILED
    DELIVERY_STATUS_SYSTEM_FAILURE, // DELIVERY_STATUS_INTERNAL_ERROR
  },
  { // DELIVERY_STATUS_IN_PROGRESS
    DELIVERY_STATUS_SYSTEM_FAILURE, // DELIVERY_EVENT_STARTING
    DELIVERY_STATUS_SYSTEM_FAILURE, // DELIVERY_EVENT_LISTENING
    DELIVERY_STATUS_COUNT, // DELIVERY_EVENT_LID_OPENED
    DELIVERY_STATUS_COMPLETE, // DELIVERY_EVENT_LID_CLOSED
    DELIVERY_STATUS_SYSTEM_FAILURE, // DELIVERY_EVENT_TRANSMITTER_FAILED
    DELIVERY_STATUS_SYSTEM_FAILURE, // DELIVERY_STATUS_INTERNAL_ERROR
  },
  { // DELIVERY_STATUS_COMPLETE
    DELIVERY_STATUS_SYSTEM_FAILURE, // DELIVERY_EVENT_STARTING
    DELIVERY_STATUS_SYSTEM_FAILURE, // DELIVERY_EVENT_LISTENING
    DELIVERY_STATUS_LID_OPEN_POST_ARRIVAL, // DELIVERY_EVENT_LID_OPENED
    DELIVERY_STATUS_COUNT, // DELIVERY_EVENT_LID_CLOSED
    DELIVERY_STATUS_SYSTEM_FAILURE, // DELIVERY_EVENT_TRANSMITTER_FAILED
    DELIVERY_STATUS_SYSTEM_FAILURE, // DELIVERY_STATUS_INTERNAL_ERROR
  },
  {
    DELIVERY_STATUS_COUNT, // DELIVERY_EVENT_STARTING
    DELIVERY_STATUS_COUNT, // DELIVERY_EVENT_LISTENING
    DELIVERY_STATUS_COUNT, // DELIVERY_EVENT_LID_OPENED
    DELIVERY_STATUS_COUNT, // DELIVERY_EVENT_LID_CLOSED
    DELIVERY_STATUS_COUNT, // DELIVERY_EVENT_TRANSMITTER_FAILED
    DELIVERY_STATUS_COUNT, // DELIVERY_STATUS_INTERNAL_ERROR
  },
};

DeliveryStatusTask::DeliveryStatusTask(TimeTask *time_task) :
    Task("Delivery status", 2048, 14),
    state(DELIVERY_STATUS_CREATED),
    time_task(time_task),
    h_delivery_event_queue(NULL),
    h_delivery_led_illumination_queue(NULL),
    h_alarm_event_queue(NULL),
    h_display_command_queue(NULL) {

}

DeliveryStatusTask::~DeliveryStatusTask() {
}

void DeliveryStatusTask::fail() {
  xQueueSendToBack(
      h_delivery_led_illumination_queue, &LED_BLINK, 0);
  xQueueSendToBack(
      h_alarm_event_queue, &TRANSMITTER_PANIC_ALARM, 0);
}

void DeliveryStatusTask::lid_is_open() {
  xQueueSendToBack(h_delivery_led_illumination_queue, &LED_BLINK, 0);
  xQueueSendToBack(h_alarm_event_queue, &LID_OPEN_ALARM, 0);
}

void DeliveryStatusTask::quiesce() {
  xQueueSendToBack(h_delivery_led_illumination_queue, &LED_OFF, 0);
  xQueueSendToBack(h_alarm_event_queue, &CONNECTED_ALARM, 0);
}

TaskHandle_t DeliveryStatusTask::start(
    QueueHandle_t h_delivery_event_queue,
    QueueHandle_t h_delivery_led_illumination_queue,
    QueueHandle_t h_alarm_event_queue,
    QueueHandle_t h_display_command_queue) {
  this->h_delivery_event_queue = h_delivery_event_queue;
  this->h_delivery_led_illumination_queue = h_delivery_led_illumination_queue;
  this->h_alarm_event_queue = h_alarm_event_queue;
  this->h_display_command_queue = h_display_command_queue;
  DeliveryEventMessage delivery_event_message;
  delivery_event_message.event = DELIVERY_EVENT_STARTING;
  xQueueSendToBack(h_delivery_event_queue, &delivery_event_message, 0);

  return create_and_start_task();
}

void DeliveryStatusTask::task_loop() {
  DeliveryEventMessage delivery_event_message;
  DisplayMessage display_message;
  for (;;) {
    if (xQueueReceive(
        h_delivery_event_queue, &delivery_event_message, portMAX_DELAY)) {
      DeliveryStatus maybe_new_state =
          TRANSITION_TABLE[state][delivery_event_message.event];
      if (maybe_new_state != DELIVERY_STATUS_COUNT) {
        switch (state = maybe_new_state) {
        case DELIVERY_STATUS_CREATED:
          // Should never happen.
          fail();
          break;
        case DELIVERY_STATUS_STARTING:
          Serial.println("Delivery status: STARTING");
          quiesce();
          break;
        case DELIVERY_STATUS_AWAITING:
          quiesce();
          break;
        case DELIVERY_STATUS_IN_PROGRESS:
          lid_is_open();
          break;
        case DELIVERY_STATUS_COMPLETE:
          time_task->start_stopwatch();
          xQueueSendToBack(h_delivery_led_illumination_queue, &LED_ON, 0);
          xQueueSendToBack(h_alarm_event_queue, &DELIVERED_ALARM, 0);
          display_message.command = LCD_DELIVERED;
          xQueueSendToBack(h_display_command_queue, &display_message, 0);
          break;
        case DELIVERY_STATUS_LID_OPEN_POST_ARRIVAL:
          lid_is_open();
          break;
        case DELIVERY_STATUS_SYSTEM_FAILURE:
          fail();
          break;
        case DELIVERY_STATUS_COUNT:
          // Should never happen.
          fail();
          break;
        }
      }
    }
  }
}
