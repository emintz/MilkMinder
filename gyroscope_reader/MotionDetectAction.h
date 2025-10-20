/*
 * GyroscopeTask.h
 *
 *  Created on: Dec 23, 2022
 *      Author: Eric Mintz
 *
 * Read the Gyroscope and send messages. The task signals tilt when the
 * tilt from the horizontal exceeds the lid open threshold.
 */

#ifndef MOTIONDETECTACTION_H_
#define MOTIONDETECTACTION_H_

#include "Arduino.h"
#include "dhtnew.h"
#include "MPU6050_light.h"
#include "PullQueueHT.h"
#include "Wire.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "MotionNotificationMessage.h"
#include "PinAssignments.h"
#include "TaskAction.h"

#define DEGREES_TO_RADIANS (PI / 180.0)
#define RADIANS_TO_DEGREES (180.0 / PI)
#define INCLINATION_THRESHOLD (PI / 6)

/**
 * Monitors the MPU6050 gyroscope to detect motion and passes the raw position
 * to the event forwarder. The lid is considered open when the gyro's Z
 * offset exceeds the lid open threshold, otherwise the lid is considered
 * closed. The notification also includes the internal gyro temperature
 * which is currently unused but included for completeness.
 */
class MotionDetectAction : public TaskAction {
  MPU6050& gyroscope_;
  DHTNEW temperature_sensor_;
  MotionNotificationMessage notification_message_;

  /**
   * The motion detection loop reads the Z acceleration, which will be 1 g
   * when the lid is level, and alerts when it exceeds INCLINATION_THRESHOLD.
   */
  PullQueueHT<MotionNotificationMessage>& gyroscope_event_queue_;

public:
  /**
   * Constructor
   *
   * Arguments
   *
   * Name                        Contents
   * --------------------------- ----------------------------------------------
   * gyroscope_event_queue       Output queue -- carries notifications to the
   *                             event forwarder
   */
  MotionDetectAction(
      PullQueueHT<MotionNotificationMessage>& gyroscope_event_queue,
      MPU6050 &gyroscope);
  virtual ~MotionDetectAction();

  /**
   * Configure the gyroscope and bind the task to its queue handle. Note
   * that the task sends gyroscope events to the specified queue.
   */
  boolean begin();

  /**
   * The motion detection loop reads the tilt angle, which will be 0
   * when the lid is level, and alerts when lid inclination equals or exceeds
   * INCLINATION_THRESHOLD. Note that the closed position is 0 degrees.
   */
  virtual void run(void) override;
};

#endif /* MOTIONDETECTACTION_H_ */
