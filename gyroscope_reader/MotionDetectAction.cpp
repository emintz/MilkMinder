/*
 * GyroscopeTask.cpp
 *
 *  Created on: Dec 23, 2022
 *      Author: Eric Mintz
 */

#include <MotionDetectAction.h>

#include "PinAssignments.h"
#include "TaskPriorities.h"

#include <cmath>

#define MPU6050_INTERRUPT_CONFIGURATION (unsigned char) 0b00011110
#define MPU6050_INTERRUPT_CONFIG_REGISTER 0x37

MotionDetectAction::MotionDetectAction(
		PullQueueHT<MotionNotificationMessage>& gyroscope_event_queue,
		MPU6050 &gyroscope) :
	gyroscope_event_queue_(gyroscope_event_queue),
	gyroscope_(gyroscope),
    temperature_sensor_(TEMPERATURE_AND_HUMIDITY_PIN) {
}

MotionDetectAction::~MotionDetectAction() {
}

boolean MotionDetectAction::begin() {
  Serial.print("Initializing gyro ... ");
  uint8_t status = gyroscope_.begin();
  Serial.print(" status (");
  Serial.print(status);
  boolean result = !status;
  Serial.println(result ? ") success." : ") failure.");
  if (result) {
    Serial.println("Calibrating ... do not move the device ...");
    vTaskDelay(pdMS_TO_TICKS(1000));
    gyroscope_.calcOffsets(true, true);
    Serial.println("... done!");
  }
  return result;
}

void MotionDetectAction::run() {
  Serial.println("Motion detection loop started.");
  Serial.print("Inclination threshold: ");
  Serial.print(INCLINATION_THRESHOLD);
  Serial.print(" radians off horizontal, ");
  Serial.print(INCLINATION_THRESHOLD * RADIANS_TO_DEGREES);
  Serial.println(" degrees.");
  for (;;) {
    float roll_in_degrees = gyroscope_.getAngleX();
    float pitch_in_degrees = gyroscope_.getAngleY();
    float tan_roll = tan(roll_in_degrees * DEGREES_TO_RADIANS);
    float tan_pitch = tan(pitch_in_degrees * DEGREES_TO_RADIANS);
    float inclination = atan(sqrt(tan_roll * tan_roll + tan_pitch * tan_pitch));

    notification_message_.temperature_celsius =
      temperature_sensor_.getTemperature();
    notification_message_.status =
        INCLINATION_THRESHOLD < inclination
            ? LID_RAISED
            : LID_HAS_NOT_MOVED;

    digitalWrite(
      BLUE_LED_PIN,
      notification_message_.status == LID_RAISED ? HIGH : LOW);
    gyroscope_event_queue_.send_message(&notification_message_, 100);
    vTaskDelay(50);
  }
}
