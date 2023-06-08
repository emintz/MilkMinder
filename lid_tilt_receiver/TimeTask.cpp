/*
 * TimeTask.cpp
 *
 *  Created on: Feb 8, 2023
 *      Author: Eric Mintz
 *
 * Acquires and publishes time of day and delivery time data.
 */

#include "TimeTask.h"

#include <stdlib.h>

#include "RTClib.h"

#include "DisplayMessage.h"

char * TimeTask::to_two_chars(uint8_t value, char *string) {
  *string++ = '0' + value/10;
  *string++ = '0' + value%10;
  return string;
}

void IRAM_ATTR TimeTask::second_tick_handler(void *params) {
  BaseType_t higher_priority_task_woken;
  vTaskNotifyGiveFromISR(
      ((IsrParams *)params)->h_time_task, &higher_priority_task_woken);
  if (higher_priority_task_woken) {
    portYIELD_FROM_ISR();
  }
}

TimeTask::TimeTask(
    RTC_DS3231 *time_keeper,
    Timezone *time_zone) :
  time_keeper(time_keeper),
  time_zone(time_zone),
  h_lcd_display(NULL),
  h_gpio_isr(NULL),
  stopwatch_state(STOPPED),
  elapsed_time_seconds(0) {
  memset(&isr_params, 0, sizeof(isr_params));

}

TimeTask::~TimeTask() {
}

void TimeTask::time_keeper_task(void *params) {
  ((TimeTask *) params)->run();
}

time_t TimeTask::now() {
  time_t current_time;
  DateTime now = time_keeper->now();
  current_time = now.unixtime();
  time_t local_time = time_zone->toLocal(current_time);
  return local_time;
}

void TimeTask::run() {
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
    xQueueSendToBack(h_lcd_display, &message, pdMS_TO_TICKS(1));

    switch (stopwatch_state) {
    case STOPPED:
      break;
    case RUNNING:
      ++elapsed_time_seconds;
      if (elapsed_time_seconds % 60 == 0) {
        memset(&message, 0, sizeof(message));
        message.command = LCD_ELAPSED;
        itoa(elapsed_time_seconds/60, message.text, DEC);
        xQueueSendToBack(h_lcd_display, &message, pdMS_TO_TICKS(1));
      }
      break;
    }
  }
}

void TimeTask::reset_stopwatch() {
  stopwatch_state = STOPPED;
  elapsed_time_seconds = 0;
}

TaskHandle_t TimeTask::start(
    QueueHandle_t h_lcd_display,
    gpio_num_t interrupt_pin) {
  this->h_lcd_display = h_lcd_display;
  bool status = time_keeper->begin();
  if (status) {
    time_keeper->writeSqwPinMode(Ds3231SqwPinMode::DS3231_SquareWave1Hz);
    // TODO: Error handling
    xTaskCreate(
        time_keeper_task,
        "Time keeper",
        4096,
        this,
        10,
        &isr_params.h_time_task);
    pinMode(interrupt_pin, INPUT_PULLUP);
    gpio_set_intr_type(interrupt_pin, GPIO_INTR_POSEDGE);
    gpio_isr_handler_add(
        interrupt_pin,
        second_tick_handler,
        &isr_params);
    Serial.println("Time task started.");
  } else {
    Serial.println("Time keeper failed to start.");
  }
  return isr_params.h_time_task;
}

void TimeTask::start_stopwatch() {
  stopwatch_state = RUNNING;
}
