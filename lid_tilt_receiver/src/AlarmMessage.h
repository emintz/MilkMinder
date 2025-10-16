/*
 * AlarmMessage.h
 *
 *  Created on: Oct 16, 2025
 *      Author: Eric Mintz
 *
 * Message that carries alarms to the alarm task
 */

#ifndef ALARMMESSAGE_H_
#define ALARMMESSAGE_H_

  /**
   * Alarm events.
   */
  enum AlarmEvent {
    ALARM_EVENT_CONNECTED,          // ESP-Now connected.
    ALARM_EVENT_DELIVERED,          // Milk has been delivered
    ALARM_EVENT_DISCONNECTED,       // ESP-Now connection lost
    ALARM_EVENT_LID_OPEN,           // Milk box lid opened
    ALARM_EVENT_TRANSMITTER_PANIC,  // Something went horribly wrong
  };
  /**
   * Message that transmits alarms. When the alarm task receives a
   * message, it signals the specified condition.
   */
  struct AlarmMessage {
    AlarmEvent event;
  };

#endif /* ALARMMESSAGE_H_ */
