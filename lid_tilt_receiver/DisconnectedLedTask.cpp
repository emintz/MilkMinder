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
    h_alarm_event_queue(NULL),
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

TaskHandle_t DisconnectedLedTask::start(QueueHandle_t h_alarm_event_queue) {
  this->h_alarm_event_queue = h_alarm_event_queue;
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
