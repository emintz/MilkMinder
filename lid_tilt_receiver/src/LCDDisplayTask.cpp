/*
 * LCDDisplayTask.cpp
 *
 *  Created on: Feb 9, 2023
 *      Author: Eric Mintz
 */

#include "LCDDisplayTask.h"

#include <stdlib.h>

#include "TimeTask.h"

LCDDisplayTask::LCDDisplayTask(
    LiquidCrystal_I2C& display,
    TimeTask *time_task,
    PullQueueHT<DisplayMessage>& display_command_queue) :
      Task(
            "LCD Display",
            4096,
            3),
      display(display),
      time_task(time_task),
      display_command_queue_(display_command_queue) {
}

LCDDisplayTask::~LCDDisplayTask() {
}

void LCDDisplayTask::connected() {
  display.setCursor(0, 1);
  display.print("OK ");
}

void LCDDisplayTask::disconnected() {
  display.setCursor(0, 1);
  display.print("NET");
}

void LCDDisplayTask::task_loop() {
  DisplayMessage command_message;
  for (;;) {
    memset(&command_message, 0, sizeof(command_message));
    if (
        display_command_queue_.pull_message(&command_message)) {
      switch (command_message.command) {
        case LCD_CLEAR:
          display.clear();
          display.setCursor(0, 0);
          break;
        case LCD_CONNECTED:
          connected();
          break;
        case LCD_DELIVERED:
          display.setCursor(0, 0);
          display.print("Delivered       ");
          connected();
          {
            char formatted_time[6];
            memset(formatted_time, 0, sizeof(formatted_time));
            tm broken_down_time;
            memset(&broken_down_time, 0, sizeof(broken_down_time));
            time_t current_time = time_task->now();
            gmtime_r(&current_time, &broken_down_time);
            char * buffer =
                time_task->to_two_chars(broken_down_time.tm_hour, formatted_time);
            *buffer++ = ':';
            buffer = time_task->to_two_chars(broken_down_time.tm_min, buffer);
            display.setCursor(16 - strlen(formatted_time), 0);
            display.print(formatted_time);
          }
          break;
        case LCD_DISCONNECTED:
          disconnected();
          break;
        case LCD_ELAPSED:
          display.setCursor(4, 1);
          display.print(command_message.text);
          break;
        case LCD_INIT:
          display.setCursor(0, 0);
          display.print("Starting        ");
          break;
        case LCD_NOOP:
          break;
        case LCD_RUN:
          display.setCursor(0, 0);
          display.print("Listening       ");
          break;
        case LCD_TIME_OF_DAY:
          display.setCursor(MAX_LCD_TEXT_LENGTH-strlen(command_message.text), 1);
          display.print(command_message.text);
          break;
        case LCD_TRANSMITTER_PANIC:
          display.setCursor(0, 0);
          display.print("XMIT FAIL       ");
          break;
        case LCD_TAMPER_ALERT:
          display.setCursor(0, 0);
          display.print("Tamper Alert    ");
          break;
        case LCD_DELIVERY_IN_PROGRESS:
          display.setCursor(0, 0);
          display.print("Milk Arriving   ");
          break;
      }
    }
  }
}

TaskHandle_t LCDDisplayTask::start(/* QueueHandle_t h_display_command */) {
  display.init();
  display.backlight();
  display.setContrast(255);
  return create_and_start_task();
}
