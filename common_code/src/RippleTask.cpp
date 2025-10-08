/*
 * RippleTask.cpp
 *
 *  Created on: Dec 29, 2022
 *      Author: Eric Mintz
 */

#include "RippleTask.h"
#include "TaskPriorities.h"

RippleTask::RippleTask(
  const uint8_t *pins,
  const size_t number_of_pins,
  const uint16_t illumination_time_ms) :
  Task(
      "Rippling Lights",
      2048,
      BLINK_TASK_PRIORITY
      ),
    pins(pins),
    number_of_pins(number_of_pins),
    illumination_time_ticks(pdMS_TO_TICKS(illumination_time_ms)),
    h_task(0) {
}

RippleTask::~RippleTask() {
}

void RippleTask::all_off() {
  const uint8_t *current_pin = pins;
  for (size_t pin_no = 0; pin_no < number_of_pins; ++pin_no) {
    digitalWrite(((unsigned char) *current_pin++), LOW);
  }
}

void RippleTask::task_loop() {
  all_off();
  for (;;) {
    const uint8_t *current_pin = pins;
    for (size_t pin_no = 0; pin_no < number_of_pins; ++pin_no) {
      all_off();
      digitalWrite(*current_pin++, HIGH);
      vTaskDelay(illumination_time_ticks);
    }
  }
}

void RippleTask::resume() {
  vTaskResume(h_task);
}

void RippleTask::start() {
  h_task = create_and_start_task();
  suspend();
}

void RippleTask::suspend() {
  vTaskSuspend(h_task);
  all_off();
}
