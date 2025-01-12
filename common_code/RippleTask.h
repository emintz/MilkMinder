/*
 * RippleTask.h
 *
 *  Created on: Dec 29, 2022
 *      Author: Eric Mintz
 *
 * Task that blinks a series of LEDs in a ripple pattern
 */

#ifndef LIBRARIES_MILKMINDER_RIPPLETASK_H_
#define LIBRARIES_MILKMINDER_RIPPLETASK_H_

#include "Arduino.h"

#include "Task.h"

class RippleTask :
    Task {

  const uint8_t *pins;
  const size_t number_of_pins;
  const TickType_t illumination_time_ticks;
  TaskHandle_t h_task;

  /**
   * Turn all LEDs off
   */
  void all_off();

  /**
   * The task loop that illuminates one LED at a time.
   */
  virtual void task_loop();

public:
  /**
   * Initializes a RippleTask instance
   *
   * Parameters:
   *
   * Name                 Contents
   * -------------------- -----------------------------------------------
   * pins                 The pins to power in time order.
   * number_of_pins       The number of pins in the array.
   * illimination_time_ms On and off time for the blink.
   */
  RippleTask(
    const uint8_t *pins,
    const size_t number_of_pins,
    const uint16_t illumination_time_ms);

  virtual ~RippleTask();

  /**
   * Resume the ripple pattern.
   */
  void resume();

  /**
   * Starts and suspends the task.
   */
  void start();

  /**
   * Suspend the ripple loop and extinguish all LEDs.
   */
  void suspend();
};

#endif /* LIBRARIES_MILKMINDER_RIPPLETASK_H_ */
