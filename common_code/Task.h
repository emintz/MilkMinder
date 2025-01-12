/*
 * Task.h
 *
 *  Created on: Feb 19, 2023
 *      Author: Eric Mintz
 *
 * Base FreeRTOS Task class, a template for implementing tasks that allocate
 * their own stacks (as opposed to tasks whose stack storage is provided
 * at startup). The task is **not** pinned to a core.
 *
 * Users must provide the following:
 *
 * 1. run_task_loop() implementation.
 *
 * 2. A startup method that invokes create_and_start_task(). Pro tip: pass
 *    all required handles via the startup method.
 */

#ifndef TASK_H_
#define TASK_H_

#include "Arduino.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

class Task {
  const char * task_name;
  const uint32_t stack_depth;
  UBaseType_t priority;
  BaseType_t creation_status;

  /**
   * The task function, the function that runs when the task starts.
   *
   * Parameters:
   *
   * Name        Contents
   * ----------- ----------------------------------------------------------
   * params      Points to the invoking Task implementation. The function
   *             invokes its task_loop method.
   */
  static void run_the_task_loop(void *params);

protected:
  Task(
      const char * task_name,
      uint32_t stack_depth,
      UBaseType_t priority);

  /**
   * Creates a FreeRTOS task that runs this instance's task loop. Returns the
   * resulting task handle or NULL if the task could not be run. Users can
   * invoke get_creation_status() to retrieve the creation status.
   */
  TaskHandle_t create_and_start_task();

public:
  virtual ~Task();

  /**
   * Returns the creation status, or 0 if the task has not started.
   */
  BaseType_t get_creation_status() {
    return creation_status;
  }

  /**
   * User-provided task loop, the method that does the work.
   */
  virtual void task_loop() = 0;
};

#endif /* TASK_H_ */
