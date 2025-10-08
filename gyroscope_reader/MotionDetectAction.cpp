/*
 * GyroscopeTask.cpp
 *
 *  Created on: Dec 23, 2022
 *      Author: Eric Mintz
 */

#include "GyroscopeTask.h"
#include "PinAssignments.h"
#include "TaskPriorities.h"

#include <cmath>

#define MPU6050_INTERRUPT_CONFIGURATION (unsigned char) 0b00011110
#define MPU6050_INTERRUPT_CONFIG_REGISTER 0x37

#define DEGREES_TO_RADIANS (PI / 180.0)
#define RADIANS_TO_DEGREES (180.0 / PI)
#define INCLINATION_THRESHOLD (PI / 6)

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
  Serial.print("Inclination threshold: ");
  Serial.print(INCLINATION_THRESHOLD);
  Serial.print(" radians, ");
  Serial.print(INCLINATION_THRESHOLD * RADIANS_TO_DEGREES);
  Serial.println(" degrees.");
  for (;;) {
    float roll_in_degrees = gyroscope.getAngleX();
    float pitch_in_degrees = gyroscope.getAngleY();
    float tan_roll = tan(roll_in_degrees * DEGREES_TO_RADIANS);
    float tan_pitch = tan(pitch_in_degrees * DEGREES_TO_RADIANS);
    float inclination = atan(sqrt(tan_roll * tan_roll + tan_pitch * tan_pitch));

    notification_message.temperature_celsius =
      temperature_sensor.getTemperature();
    notification_message.status =
        INCLINATION_THRESHOLD < inclination
            ? LID_RAISED
            : LID_HAS_NOT_MOVED;

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
    taskYIELD();
  }
}

TaskHandle_t GyroscopeTask::UpdateTask::start(MPU6050 *gyroscope) {
  this->gyroscope = gyroscope;
  return create_and_start_task();
}
