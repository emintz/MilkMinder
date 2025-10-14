/*
 * MilkArrivalTask.cpp
 *
 *  Created on: Apr 4, 2023
 *      Author: Eric Mintz
 */

#include "MilkArrivalTask.h"

#include "PinAssignments.h"
#include "WhiteLedPin.h"

// Lid open confirmation time in milliseconds. When the lid is held open
// for the confirmation time, delivery has definitely started.
#define CONFIRM_OPEN_TIMEOUT_TICKS pdMS_TO_TICKS(500)

// Lid closure confirmation time. When the lid has been closed for the
// specified time, delivery has definitely ended.
#define CONFIRM_CLOSURE_TIMEOUT_TICKS pdMS_TO_TICKS(5000)

static const AlarmTask::AlarmTaskMessage CONNECTED_ALARM = {
  AlarmTask::ALARM_EVENT_CONNECTED
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

MilkArrivalTask::MilkArrivalTask(
    TimeTask *time_task,
    PullQueueHT<AlarmTask::AlarmTaskMessage>& alarm_event_queue,
    PullQueueHT<LedIlluminationMessage>& delivery_led_illumination_queue,
    PullQueueHT<DisplayMessage>& display_command_queue,
    PullQueueHT<LidPositionReport>& lid_position_report_queue) :
      Task(
          "Milk Arrival",
          2048,
          9),
      time_task(time_task),
      alarm_event_queue_(alarm_event_queue),
      delivery_led_illumination_queue_(delivery_led_illumination_queue),
      display_command_queue_(display_command_queue),
      lid_position_report_queue_(lid_position_report_queue),
      h_lid_position_report_queue_(NULL),
      state_(ArrivalState::MILK_ARRIVAL_CRREATED),
      on_timeout_(lid_position_report_queue),
      event_timer_(
          "ArrivalTimer",
          on_timeout_){
}

MilkArrivalTask::~MilkArrivalTask(void) {
}

TaskHandle_t MilkArrivalTask::start(void) {
  this-> h_lid_position_report_queue_ = h_lid_position_report_queue_;
  on_timeout_.begin();
  event_timer_.begin();
  return create_and_start_task();
};

void MilkArrivalTask::halt_countdown() {
  on_timeout_.set_timeout_report(LidPositionReport::LID_POS_UNCHANGED);
  event_timer_.stop();
}

void MilkArrivalTask::lid_is_open() {
  delivery_led_illumination_queue_.send_message(&LED_BLINK, 0);
  alarm_event_queue_.send_message(&LID_OPEN_ALARM, 0);
}

void MilkArrivalTask::quiesce() {
  delivery_led_illumination_queue_.send_message(&LED_OFF, 0);
  alarm_event_queue_.send_message(&CONNECTED_ALARM, 0);
}

void MilkArrivalTask::start_countdown(
    TickType_t timeout,
    LidPositionReport::PositionValue notification_on_expiration) {
  halt_countdown();
//  old_timeout_action_.set_timeout_report(notification_on_expiration);
//  old_timer_.start(timeout);
  on_timeout_.set_timeout_report(notification_on_expiration);
  event_timer_.start_ticks(timeout);
}

void MilkArrivalTask::task_loop() {
  LidPositionReport position_report;
  DisplayMessage display_message;
  uint8_t led_level = LOW;
  Serial.println("Milk arrival task started.");
  for (;;) {
    if (lid_position_report_queue_.pull_message(&position_report)) {
      ArrivalState maybe_new_state =
          STATE_TRANSITION_TABLE[state_][position_report.lid_position];
      if (maybe_new_state != MILK_ARRIVAL_NUMBER_OF_STATES) {
        led_level = LOW;
        switch (state_ = maybe_new_state) {
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
          display_command_queue_.send_message(&display_message, 0);
          break;
        case ArrivalState::MILK_ARRIVAL_SUSPECT_DELIVERY_IS_COMPLETE:
          start_countdown(
              CONFIRM_CLOSURE_TIMEOUT_TICKS,
              LidPositionReport::LID_POS_CLOSE_TIMEOUT);
          break;
        case ArrivalState::MILK_ARRIVAL_CONFIRMED_DELIVERY_IS_COMPLETE:
          time_task->start_stopwatch();
          delivery_led_illumination_queue_.send_message(&LED_ON, 0);
          alarm_event_queue_.send_message(&DELIVERED_ALARM, 0);
          display_message.command = LCD_DELIVERED;
          display_command_queue_.send_message(&display_message, 0);
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
          display_command_queue_.send_message(&display_message, 0);
          break;
        case ArrivalState::MILK_ARRIVAL_NUMBER_OF_STATES:
          break;
        }
        digitalWrite(WHITE_LED_PIN, led_level);
      }
    }
  }
}
