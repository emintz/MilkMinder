/*
 * BlinkTask.cpp
 *
 *  Created on: Dec 25, 2022
 *      Author: Eric Mintz
 */

#include "BlinkTask.h"

BlinkTask::BlinkTask(
    const char *name,
    uint16_t led_pin,
    uint16_t number_of_flashes,
    uint16_t inter_flash_wait_ms,
    uint16_t inter_group_wait_ms) :
  Task(name, 1024, 3),
  led_pin(led_pin),
  number_of_flashes(number_of_flashes),
  inter_flash_wait_ticks(pdMS_TO_TICKS(inter_flash_wait_ms)),
  inter_group_wait_ticks(pdMS_TO_TICKS(inter_group_wait_ms)),
  h_this_task(0) {
}

BlinkTask::~BlinkTask() {
}

void BlinkTask::task_loop() {
  digitalWrite(led_pin, LOW);
  for (;;) {
    TickType_t off_time = 0;
    for (uint16_t i = 0; i < number_of_flashes; ++i) {
      vTaskDelay(off_time);
      off_time = inter_flash_wait_ticks;
      digitalWrite(led_pin, HIGH);
      vTaskDelay(inter_flash_wait_ticks);
      digitalWrite(led_pin, LOW);
    }
    vTaskDelay(inter_group_wait_ticks);
  }
}

TaskHandle_t BlinkTask::start_blink_loop(const char *name) {
  h_this_task = create_and_start_task();
  suspend();
  return h_this_task;
}

void BlinkTask::suspend() {
  vTaskSuspend(h_this_task);
  digitalWrite(led_pin, LOW);
}

void BlinkTask::resume() {
  vTaskResume(h_this_task);
}

void BlinkTask::resume(
    uint16_t number_of_flashes,
    TickType_t inter_flash_wait_ticks,
    TickType_t inter_group_wait_ticks) {
  this->number_of_flashes = number_of_flashes;
  this->inter_flash_wait_ticks = inter_flash_wait_ticks;
  this->inter_group_wait_ticks = inter_group_wait_ticks;
  resume();
}
