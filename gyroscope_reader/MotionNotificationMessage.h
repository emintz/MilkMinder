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

#define ABSOLUTE_ZERO -273.15

enum MotionStatus {
  LID_HAS_NOT_MOVED,
  LID_RAISED,
  GYROSCOPE_SIGNAL_LOST,
  PING,
  LID_CLOSURE_TIMEOUT,  // For receiver to debounce lid closure.
  LAST_NOTIFICATION_STATUS,  // MUST be last.
};

struct MotionNotificationMessage {
  MotionStatus status;
  float temperature_celsius;
};

#endif /* MOTIONNOTIFICATIONMESSAGE_H_ */
