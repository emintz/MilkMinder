/*
 * GyroscopeTask.cpp
 *
 *  Created on: Dec 23, 2022
 *      Author: Eric Mintz
 */

#include "GyroscopeTask.h"
#include "PinAssignments.h"
#include "TaskPriorities.h"

static TaskHandle_t h_motion_detection_task = 0;

#define MPU6050_INTERRUPT_CONFIGURATION (unsigned char) 0b00011110
#define MPU6050_INTERRUPT_CONFIG_REGISTER 0x37

void IRAM_ATTR motion_detected_interrupt_handler() {
  if (h_motion_detection_task) {
    BaseType_t higher_priority_task_taken = pdFALSE;
    vTaskNotifyGiveFromISR(
      h_motion_detection_task,
      &higher_priority_task_taken);
    portYIELD_FROM_ISR(higher_priority_task_taken);
  }
}

GyroscopeTask::GyroscopeTask() :
    Task(
        "MPU6050 motion detection loop",
        2048,
        MOTION_DETECTION_PRIORITY),
    update_task(),
    h_gyro_event_queue(NULL),
    gyroscope(Wire),
    temperature_sensor(TEMPERATURE_AND_HUMIDITY_PIN) {
}

GyroscopeTask::~GyroscopeTask() {
}

boolean GyroscopeTask::begin(QueueHandle_t h_gyro_event_queue) {
  this->h_gyro_event_queue = h_gyro_event_queue;
  Serial.print("Initializing gyro ... ");
  uint8_t status = gyroscope.begin();
  Serial.print(" status (");
  Serial.print(status);
  boolean result = !status;
  Serial.println(result ? ") success." : ") failure.");
  if (result) {
    Serial.println("Calibrating ... do not move the device ...");
    vTaskDelay(pdMS_TO_TICKS(1000));
    gyroscope.calcOffsets(true, true);
    Serial.println("... done!");
  }
  return result;
}

TaskHandle_t GyroscopeTask::start_update_loop() {
  return update_task.start(&gyroscope);
}

void GyroscopeTask::task_loop() {
  Serial.println("Motion detection loop started.");
  for (;;) {
    float z_acceleration = gyroscope.getAccZ();
    notification_message.temperature_celsius =
      temperature_sensor.getTemperature();
    notification_message.status =
        z_acceleration <= .9 ? LID_RAISED : LID_HAS_NOT_MOVED;
    digitalWrite(
      BLUE_LED_PIN,
      notification_message.status == LID_RAISED ? HIGH : LOW);
    xQueueSendToBack(
      h_gyro_event_queue,
      &notification_message, pdMS_TO_TICKS(100));
    vTaskDelay(50);
  }
}

TaskHandle_t GyroscopeTask::start_motion_detection_loop() {
  Serial.println("Starting notification loop.");
  return create_and_start_task();
}

GyroscopeTask::UpdateTask::UpdateTask() :
    Task(
        "MPU6050 update loop",
        2048,
        GYROSCOPE_UPDATE_PIORITY),
        gyroscope(NULL) {
}

GyroscopeTask::UpdateTask::~UpdateTask() {
}

void GyroscopeTask::UpdateTask::task_loop() {
  Serial.println("GyroscopeTask::UpdateTask::task_loop() started");
  for (;;) {
    gyroscope->update();
    vTaskDelay(pdMS_TO_TICKS(2));
  }
}

TaskHandle_t GyroscopeTask::UpdateTask::start(MPU6050 *gyroscope) {
  this->gyroscope = gyroscope;
  return create_and_start_task();
}
