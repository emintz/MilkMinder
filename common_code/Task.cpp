/*
 * Task.cpp
 *
 *  Created on: Feb 19, 2023
 *      Author: Eric Mintz
 */

#include "Task.h"

Task::Task(
    const char * task_name,
    uint32_t stack_depth,
    UBaseType_t priority) :
      task_name(task_name),
      stack_depth(stack_depth),
      priority(priority),
      creation_status(0) {

}

Task::~Task() {
}

TaskHandle_t Task::create_and_start_task() {
  TaskHandle_t task_handle = NULL;

  creation_status = xTaskCreate(
      run_the_task_loop,
      task_name,
      stack_depth,
      this,
      priority,
      &task_handle);

  if (creation_status != pdPASS) {
    task_handle = NULL;
  }

  return task_handle;
}

void Task::run_the_task_loop(void *params) {
  ((Task *) params)->task_loop();
}
