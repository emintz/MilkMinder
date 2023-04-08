/*
 * MilkArrivalTask.cpp
 *
 *  Created on: Apr 4, 2023
 *      Author: Eric Mintz
 */

#include "MilkArrivalTask.h"

#include "LidPositionReport.h"

#include "AlarmTask.h"
#include "DeliveryLEDIlluminationStatus.h"
#include "DisplayMessage.h"
#include "PinAssignments.h"
#include "WhiteLedPin.h"

// Lid open confirmation time in milliseconds. When the lid is held open
// for the confirmation time, delivery has definitely started.
#define CONFIRM_OPEN_TIMEOUT_TICKS pdMS_TO_TICKS(500)

// Lid closure confirmation time. When the lid has been closed for the
// specified time, delivery has definitely ended.
#define CONFIRM_CLOSURE_TIMEOUT_TICKS pdMS_TO_TICKS(5000)

static const AlarmTask::AlarmTaskMessage CONNECTED_ALARM =
    { AlarmTask::ALARM_EVENT_CONNECTED
};
static const AlarmTask::AlarmTaskMessage DELIVERED_ALARM = {
    AlarmTask::ALARM_EVENT_DELIVERED
};
static const AlarmTask::AlarmTaskMessage LID_OPEN_ALARM = {
    AlarmTask::ALARM_EVENT_LID_OPEN
};

static const LedIlluminationMessage LED_OFF = { DELIVERY_LED_OFF };
static const LedIlluminationMessage LED_BLINK = { DELIVERY_LED_BLINK };
static const LedIlluminationMessage LED_ON = { DELIVERY_LED_ON };

MilkArrivalTask::ArrivalState MilkArrivalTask::STATE_TRANSITION_TABLE
    [MilkArrivalTask::MILK_ARRIVAL_NUMBER_OF_STATES]
    [LidPositionReport::LID_POS_NUMBER_OF_VALUES] = {
      { // MILK_ARRIVAL_CRREATED
        MILK_ARRIVAL_NUMBER_OF_STATES,   // LID_POS_UNCHANGED
        MILK_ARRIVAL_SUSPECT_DELIVERY_HAS_BEGUN, // LID_POS_OPEN
        MILK_ARRIVAL_WAITING_FOR_ARRIVAL, // LID_POS_CLOSED
        MILK_ARRIVAL_NUMBER_OF_STATES, // LID_POS_OPEN_TIMEOUT
        MILK_ARRIVAL_NUMBER_OF_STATES,  // LID_POS_CLOSE_TIMEOUT
      },
      { // MILK_ARRIVAL_WAITING_FOR_ARRIVAL
        MILK_ARRIVAL_NUMBER_OF_STATES, // LID_POS_UNCHANGED
        MILK_ARRIVAL_SUSPECT_DELIVERY_HAS_BEGUN, // LID_POS_OPEN
        MILK_ARRIVAL_NUMBER_OF_STATES, // LID_POS_CLOSED
        MILK_ARRIVAL_NUMBER_OF_STATES, // LID_POS_OPEN TIMEOUT
        MILK_ARRIVAL_NUMBER_OF_STATES, // LID_POS_CLOSE TIMEOUT
      },
      { // MILK_ARRIVAL_SUSPECT_DELIVERY_HAS_BEGUN
        MILK_ARRIVAL_NUMBER_OF_STATES, // LID_POS_UNCHANGED
        MILK_ARRIVAL_NUMBER_OF_STATES,  // LID_POS_OPEN
        MILK_ARRIVAL_WAITING_FOR_ARRIVAL,  // LID_POS_CLOSED
        MILK_ARRIVAL_CONFIRMED_DELEVERY_HAS_BEGUN,  // LID_POS_OPEN_TIMEOUT
        MILK_ARRIVAL_NUMBER_OF_STATES, // LID_POS_CLOSE TIMEOUT
      },
      { // MILK_ARRIVAL_CONFIRMED_DELEVERY_HAS_BEGUN
        MILK_ARRIVAL_NUMBER_OF_STATES, // LID_POS_UNCHANGED
        MILK_ARRIVAL_NUMBER_OF_STATES, // LID_POS_OPEN
        MILK_ARRIVAL_SUSPECT_DELIVERY_IS_COMPLETE, // LID_POS_CLOSED
        MILK_ARRIVAL_NUMBER_OF_STATES, // LID_POS_OPEN_TIMEOUT
        MILK_ARRIVAL_NUMBER_OF_STATES, // LID_POS_CLOSE_TIMEOUT
      },
      { // MILK_ARRIVAL_SUSPECT_DELIVERY_IS_COMPLETE
        MILK_ARRIVAL_NUMBER_OF_STATES, // LID_POS_UNCHANGED
        MILK_ARRIVAL_NUMBER_OF_STATES, // LID_POS_OPEN
        MILK_ARRIVAL_NUMBER_OF_STATES, // LID_POS_CLOSED
        MILK_ARRIVAL_NUMBER_OF_STATES, // LID_POS_OPEN_TIMEOUT
        MILK_ARRIVAL_CONFIRMED_DELIVERY_IS_COMPLETE, // LID_POS_CLOSE_TIMEOUT
      },
      { // MILK_ARRIVAL_CONFIRMED_DELIVERY_IS_COMPLETE
        MILK_ARRIVAL_NUMBER_OF_STATES, // LID_POS_UNCHANGED
        MILK_ARRIVAL_SUSPECT_TAMPERING, // LID_POS_OPEN
        MILK_ARRIVAL_NUMBER_OF_STATES, // LID_POS_CLOSED
        MILK_ARRIVAL_NUMBER_OF_STATES, // LID_POS_OPEN_TIMEOUT
        MILK_ARRIVAL_NUMBER_OF_STATES, // LID_POS_CLOSE_TIMEOUT
      },
      { // MILK_ARRIVAL_SUSPECT_TAMPERING
        MILK_ARRIVAL_NUMBER_OF_STATES, // LID_POS_UNCHANGED
        MILK_ARRIVAL_NUMBER_OF_STATES, // LID_POS_OPEN
        MILK_ARRIVAL_CONFIRMED_DELIVERY_IS_COMPLETE, // LID_POS_CLOSED
        MILK_ARRIVAL_CONFIRMED_TAMPERING, // LID_POS_OPEN_TIMEOUT
        MILK_ARRIVAL_NUMBER_OF_STATES, // LID_POS_CLOSE_TIMEOUT
      },
      { // MILK_ARRIVAL_CONFIRMED_TAMPERING
        MILK_ARRIVAL_NUMBER_OF_STATES, // LID_POS_UNCHANGED
        MILK_ARRIVAL_NUMBER_OF_STATES, // LID_POS_OPEN
        MILK_ARRIVAL_NUMBER_OF_STATES, // LID_POS_CLOSED
        MILK_ARRIVAL_NUMBER_OF_STATES, // LID_POS_OPEN_TIMEOUT
        MILK_ARRIVAL_NUMBER_OF_STATES, // LID_POS_CLOSE_TIMEOUT
       },
    };

MilkArrivalTask::MilkArrivalTask(TimeTask *time_task) :
  Task(
      "Milk Arrival",
      2048,
      9),
  time_task(time_task),
  h_lid_position_report_queue(NULL),
  h_delivery_led_illumination_queue(NULL),
  h_alarm_event_queue(NULL),
  h_display_command_queue(NULL),
  state(ArrivalState::MILK_ARRIVAL_CRREATED),
  timeout_action(),
  timer("Milk Arrival Timer", &timeout_action) {
}

MilkArrivalTask::~MilkArrivalTask() {
}

TaskHandle_t MilkArrivalTask::start(
    QueueHandle_t h_lid_position_report_queue,
    QueueHandle_t h_delivery_led_illumination_queue,
    QueueHandle_t h_alarm_event_queue,
    QueueHandle_t h_display_command_queue) {
  this-> h_lid_position_report_queue = h_lid_position_report_queue;
  this->h_delivery_led_illumination_queue = h_delivery_led_illumination_queue;
  this->h_alarm_event_queue = h_alarm_event_queue;
  this->h_display_command_queue = h_display_command_queue;
  timeout_action.begin(h_lid_position_report_queue);

  return create_and_start_task();
};

void MilkArrivalTask::halt_countdown() {
  timeout_action.set_timeout_report(LidPositionReport::LID_POS_UNCHANGED);
  timer.stop();
}

void MilkArrivalTask::lid_is_open() {
  xQueueSendToBack(h_delivery_led_illumination_queue, &LED_BLINK, 0);
  xQueueSendToBack(h_alarm_event_queue, &LID_OPEN_ALARM, 0);
}

void MilkArrivalTask::quiesce() {
  xQueueSendToBack(h_delivery_led_illumination_queue, &LED_OFF, 0);
  xQueueSendToBack(h_alarm_event_queue, &CONNECTED_ALARM, 0);
}

void MilkArrivalTask::start_countdown(
    TickType_t timeout,
    LidPositionReport::PositionValue notification_on_expiration) {
  halt_countdown();
  timeout_action.set_timeout_report(notification_on_expiration);
  timer.start(timeout);
}

void MilkArrivalTask::task_loop() {
  LidPositionReport position_report;
  DisplayMessage display_message;
  uint8_t led_level = LOW;
  Serial.println("Milk arrival task started.");
  for (;;) {
    if (xQueueReceive(
        h_lid_position_report_queue, &position_report, portMAX_DELAY)) {
      ArrivalState maybe_new_state =
          STATE_TRANSITION_TABLE[state][position_report.lid_position];
      if (maybe_new_state != MILK_ARRIVAL_NUMBER_OF_STATES) {
        led_level = LOW;
        switch (state = maybe_new_state) {
        case ArrivalState::MILK_ARRIVAL_CRREATED:
          // For the sake of completeness, as there are no transitions
          // into this state.
          break;
        case ArrivalState::MILK_ARRIVAL_WAITING_FOR_ARRIVAL:
          halt_countdown();
          break;
        case ArrivalState::MILK_ARRIVAL_SUSPECT_DELIVERY_HAS_BEGUN:
          led_level = HIGH;
          start_countdown(
              CONFIRM_OPEN_TIMEOUT_TICKS,
              LidPositionReport::LID_POS_OPEN_TIMEOUT);
          break;
        case ArrivalState::MILK_ARRIVAL_CONFIRMED_DELEVERY_HAS_BEGUN:
          led_level = HIGH;
          lid_is_open();
          display_message.command = LCD_DELIVERY_IN_PROGRESS;
          xQueueSendToBack(h_display_command_queue, &display_message, 0);
          break;
        case ArrivalState::MILK_ARRIVAL_SUSPECT_DELIVERY_IS_COMPLETE:
          start_countdown(
              CONFIRM_CLOSURE_TIMEOUT_TICKS,
              LidPositionReport::LID_POS_CLOSE_TIMEOUT);
          break;
        case ArrivalState::MILK_ARRIVAL_CONFIRMED_DELIVERY_IS_COMPLETE:
          time_task->start_stopwatch();
          xQueueSendToBack(h_delivery_led_illumination_queue, &LED_ON, 0);
          xQueueSendToBack(h_alarm_event_queue, &DELIVERED_ALARM, 0);
          display_message.command = LCD_DELIVERED;
          xQueueSendToBack(h_display_command_queue, &display_message, 0);
          break;
        case ArrivalState::MILK_ARRIVAL_SUSPECT_TAMPERING:
          led_level = HIGH;
          start_countdown(
              CONFIRM_OPEN_TIMEOUT_TICKS,
              LidPositionReport::LID_POS_OPEN_TIMEOUT);
          break;
        case ArrivalState::MILK_ARRIVAL_CONFIRMED_TAMPERING:
          led_level = HIGH;
          lid_is_open();
          display_message.command = LCD_TAMPER_ALERT;
          xQueueSendToBack(h_display_command_queue, &display_message, 0);
          break;
        case ArrivalState::MILK_ARRIVAL_NUMBER_OF_STATES:
          break;
        }
        digitalWrite(WHITE_LED_PIN, led_level);
      }
    }
  }
}
