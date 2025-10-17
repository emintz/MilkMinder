/*
 * TimeAction.cpp
 *
 *  Created on: Feb 8, 2023
 *      Author: Eric Mintz
 *
 * Acquires and publishes time of day and delivery time data.
 */

#include <src/TimeAction.h>
#include <stdlib.h>

#include "Priorities.h"
#include "RTClib.h"


char * TimeAction::to_two_chars(uint8_t value, char *string) {
  *string++ = '0' + value/10;
  *string++ = '0' + value%10;
  return string;
}

void IRAM_ATTR TimeAction::second_tick_handler(void *params) {
  static_cast<TimeAction *>(params)->notify_from_isr();
}

TimeAction::TimeAction(
    RTC_DS3231 *time_keeper,
    gpio_num_t interrupt_pin,
    Timezone *time_zone,
    PullQueueHT<DisplayMessage>& display_command_queue) :
      time_keeper_(time_keeper),
      interrupt_pin_(interrupt_pin),
      time_zone_(time_zone),
      display_command_queue_(display_command_queue),
      h_gpio_isr_(NULL),
      stopwatch_state_(STOPPED),
      elapsed_time_seconds_(0) {
}

TimeAction::~TimeAction() {
}

time_t TimeAction::now() {
  time_t current_time;
  DateTime now = time_keeper_->now();
  current_time = now.unixtime();
  time_t local_time = time_zone_->toLocal(current_time);
  return local_time;
}

void TimeAction::run() {
  DisplayMessage message;
  for (;;) {
    memset(&message, 0, sizeof(message));
    message.command = LCD_TIME_OF_DAY;
    ulTaskNotifyTake(true, portMAX_DELAY);
    tm broken_down_time;
    time_t current_time = now();
    gmtime_r(&current_time, &broken_down_time);
    char * buffer = to_two_chars(broken_down_time.tm_hour, message.text);
    *buffer++ = ':';
    buffer = to_two_chars(broken_down_time.tm_min, buffer);
    *buffer++ = ':';
    to_two_chars(broken_down_time.tm_sec, buffer);
    display_command_queue_.send_message(&message, pdMS_TO_TICKS(1));
    switch (stopwatch_state_) {
    case STOPPED:
      break;
    case RUNNING:
      ++elapsed_time_seconds_;
      if (elapsed_time_seconds_ % 60 == 0) {
        memset(&message, 0, sizeof(message));
        message.command = LCD_ELAPSED;
        itoa(elapsed_time_seconds_/60, message.text, DEC);
        display_command_queue_.send_message(&message, pdMS_TO_TICKS(1));
      }
      break;
    }
  }
}

void TimeAction::reset_stopwatch() {
  stopwatch_state_ = STOPPED;
  elapsed_time_seconds_ = 0;
}

bool TimeAction::begin() {
  Serial.println("Starting the timer task.");
  bool status = time_keeper_->begin();
  if (status) {
    time_keeper_->writeSqwPinMode(Ds3231SqwPinMode::DS3231_SquareWave1Hz);
    pinMode(interrupt_pin_, INPUT_PULLUP);
    status =
        ESP_OK == gpio_set_intr_type(interrupt_pin_, GPIO_INTR_POSEDGE)
        && ESP_OK == gpio_isr_handler_add(
            interrupt_pin_,
            second_tick_handler,
            this);
  }

  Serial.println(status
      ? "Time task started."
      : "Time keeper failed to start.");
  return status;
}

void TimeAction::start_stopwatch() {
  stopwatch_state_ = RUNNING;
}
