/*
 * DisconnectedLedTask.cpp
 *
 *  Created on: Feb 20, 2023
 *      Author: Eric Mintz
 */

#include "DisconnectedLedTask.h"

DisconnectedLedTask::DisconnectedLedTask(
  uint8_t led_pin) :
    Task("Disconnect Blink", 2048, 5),
    h_task(NULL),
    led_pin(led_pin) {
}

DisconnectedLedTask::~DisconnectedLedTask() {
}

void DisconnectedLedTask::disable() {
  vTaskSuspend(h_task);
  digitalWrite(led_pin, LOW);
}

void DisconnectedLedTask::enable() {
  vTaskResume(h_task);
}

TaskHandle_t DisconnectedLedTask::start() {
  return h_task = create_and_start_task();
}

void DisconnectedLedTask::task_loop() {
  Serial.println("Disconnected LED Task started.");
  for (;;) {
    digitalWrite(led_pin, HIGH);
    vTaskDelay(100);
    digitalWrite(led_pin, LOW);
    vTaskDelay(100);
  }
}
