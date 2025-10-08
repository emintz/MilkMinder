/*
 * TimeTask.h
 *
 *  Created on: Feb 8, 2023
 *      Author: Eric Mintz
 *
 * Tracks the current time using a DS3231 time source.
 */

#ifndef TIMETASK_H_
#define TIMETASK_H_

#include "Arduino.h"

#include <time.h>
#include <sys/time.h>

#include "driver/gpio.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "Timezone.h"

class RTC_DS3231;

class TimeTask {
  enum State {
    STOPPED,
    RUNNING,
  };


  struct IsrParams {
    TaskHandle_t h_time_task;
  };

  RTC_DS3231 *time_keeper;
  Timezone *time_zone;
  QueueHandle_t h_lcd_display;
  gpio_isr_handle_t h_gpio_isr;
  IsrParams isr_params;
  State stopwatch_state;
  uint16_t elapsed_time_seconds;

  static void IRAM_ATTR second_tick_handler(void *params);

  /**
   * Task start function.
   *
   * Parameters:
   * ----------
   *
   * Name                Contents
   * ------------------- ----------------------------------------------------
   * params              Pointer to the TimeTask to start
   */
  static void time_keeper_task(void *params);

  /**
   * Task run loop.
   */
  void run();

public:
  TimeTask(
      RTC_DS3231 *time_keeper,
      Timezone *time_zone);
  virtual ~TimeTask();

  time_t now();

  void reset_stopwatch();

  TaskHandle_t start(
      QueueHandle_t h_lcd_display,
      gpio_num_t interrupt_pin);

  void start_stopwatch();

  char * to_two_chars(uint8_t value, char *string);
};

#endif /* TIMETASK_H_ */
