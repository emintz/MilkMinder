/*
 * AlarmAction.h
 *
 *  Created on: Feb 15, 2023
 *      Author: Eric Mintz
 *
 * Task that manages the alarm, a.k.a. the beeper, and a specified LED.
 * Note that the LED illuminates when the beeper sounds.
 */

#ifndef ALARMACTION_H_
#define ALARMACTION_H_

#include "PullQueueHT.h"

#include "TaskAction.h"
#include "AlarmMessage.h"


class AlarmAction : public TaskAction {
public:
  /**
   * A component of a signal, a pin level (HIGH or LOW) and the length of
   * time to maintain it.
   */
  struct LevelAndDuration {
    uint8_t level;   // HIGH or LOW
    uint16_t duration_ms;  // Milliseconds to hold the pin to the level.
  };

  /**
   * An alarm signal, a sequence of levels and durations to emit.
   */
  struct AlarmSignal {
    size_t level_count;  // Number of levels comprising the alarm
    const LevelAndDuration *level;
  };
private:
  const uint8_t audio_alert_pin_no;
  const uint8_t led_pin_no;
  PullQueueHT<AlarmMessage>& alarm_event_queue_;

  /**
   * Emits the specified alarm until a user requests another.
   */
  void emit_alarm(const AlarmSignal &alarm_signal);

public:
  /**
   * Constructor
   *
   * Parameters:
   *
   * Name                Contents
   * ------------------  ------------------------------------------------------
   * audio_alert_pin_no  The GPIO pin that is connected to the alarm beeper.
   * led_pin_no          The GPIO pin that is connected to the alarm LED.
   * alarm_event_queue   Pull queue carrying the inbound alarm events which
   *                     this class processes
   */
  AlarmAction(
      uint8_t audio_alert_pin_no,
      uint8_t led_pin_no,
      PullQueueHT<AlarmMessage>& alarm_event_queue);

  virtual ~AlarmAction();

  /**
   * The task loop, which listens for incoming alarm messages and emits
   * the requested alarms by publishing AlarmTaskMessage instance to the
   * queue.
   */
  virtual void run() override;
};

#endif /* ALARMACTION_H_ */
