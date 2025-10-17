/*
 * TimeAction.h
 *
 *  Created on: Feb 8, 2023
 *      Author: Eric Mintz
 *
 * Tracks the current time using a DS3231 time source.
 */

#ifndef TIMEACTION_H_
#define TIMEACTION_H_

#include "Arduino.h"

#include <time.h>
#include <sys/time.h>

#include "driver/gpio.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "DisplayMessage.h"
#include "PullQueueHT.h"
#include "TaskAction.h"
#include "Timezone.h"

class RTC_DS3231;

class TimeAction : public TaskAction {
  enum State {
    STOPPED,
    RUNNING,
  };

  RTC_DS3231 *time_keeper_;
  const gpio_num_t interrupt_pin_;
  Timezone *time_zone_;
  PullQueueHT<DisplayMessage>& display_command_queue_;
  gpio_isr_handle_t h_gpio_isr_;
  State stopwatch_state_;
  uint16_t elapsed_time_seconds_;

  static void IRAM_ATTR second_tick_handler(void *params);

public:
  /**
   * Constructor
   *
   * Parameters:
   * ----------
   *
   * Name                  Contents
   * --------------------- ----------------------------------------------------
   * time_keeper           The real time clock
   * interrupt_pin         GPIO that receives the 1 Hz square wave from the
   *                       real time clock.
   * time_zone             The time zone in which the milk arrives
   * display_command_queue Time changes are posted here.
   */
  TimeAction(
      RTC_DS3231 *time_keeper,
      gpio_num_t interrupt_pin,
      Timezone *time_zone,
      PullQueueHT<DisplayMessage>& display_command_queue);
  virtual ~TimeAction();

  /*
   * Starts the time source and installs the per-second interrupt
   * handler. Be sure to invoke this before starting the timer task.
   */
  bool begin();

  /**
   * Returns the number of seconds since the epoch start
   */
  time_t now();

  /**
   * Resets the milk arrival stopwatch. See start_stopwatch().
   */
  void reset_stopwatch();

  /**
   * Task run loop.
   */
  void run();

  TaskHandle_t start(void);

  /**
   * Starts the milk arrival stopwatch that accumulates the number of minutes
   * since delivery.
   */
  void start_stopwatch();

//  virtual void task_loop() override;

  char * to_two_chars(uint8_t value, char *string);
};

#endif /* TIMEACTION_H_ */
