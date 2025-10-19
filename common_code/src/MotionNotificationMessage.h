/*
 * MotionNotificationMessage.h
 *
 *  Created on: Dec 24, 2022
 *      Author: Eric Mintz
 *
 * Motion notification message, which the gyroscope reader sends periodically
 */

#ifndef MOTIONNOTIFICATIONMESSAGE_H_
#define MOTIONNOTIFICATIONMESSAGE_H_

#include "freertos/FreeRTOS.h"

#define ABSOLUTE_ZERO -273.15

enum MotionStatus {
  LID_HAS_NOT_MOVED,
  LID_RAISED,
  GYROSCOPE_SIGNAL_LOST,
  PING,
  LAST_NOTIFICATION_STATUS,  // MUST be last.
};

struct MotionNotificationMessage {
  /**
   * Event type: what the lid did, including
   * nothing at all.
   */
  MotionStatus status;
  /**
   * Gyroscope temperature
   */
  float temperature_celsius;
  /**
   * The time in milliseconds since the lid first opened, or zero if
   * the lid has not opened.
   */
  unsigned long when_opened;
  /**
   * The number of times the lid has been opened
   */
  uint16_t opened_count;
};

#endif /* MOTIONNOTIFICATIONMESSAGE_H_ */
