/*
 * AlarmTask.h
 *
 *  Created on: Feb 15, 2023
 *      Author: Eric Mintz
 *
 * Task that manages the alarm, a.k.a. the beeper
 */

#ifndef ALARMTASK_H_
#define ALARMTASK_H_

#include "Task.h"

class AlarmTask :
    public Task {
public:
  /**
   * Types of available signal.
   */
  enum Event {
    ALARM_EVENT_CONNECTED,
    ALARM_EVENT_DELIVERED,
    ALARM_EVENT_DISCONNECTED,
    ALARM_EVENT_LID_OPEN,
    ALARM_EVENT_TRANSMITTER_PANIC,
  };
  /**
   * Message used to transmit commands. When the alarm task receives a
   * message, it emits the specified signal.
   */
  struct AlarmTaskMessage {
    Event event;
  };

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
  QueueHandle_t h_alarm_event_queue;
  const uint8_t audio_alert_pin_no;
  const uint8_t led_pin_no;

  /**
   * Emits the specified alarm until a user requests another.
   */
  void emit_alarm(const AlarmSignal &alarm_signal);

  /**
   * The task loop, which listens for incoming alarm messages and emits
   * the requested alarms by publishing AlarmTaskMessage instance to the
   * queue.
   */
  void task_loop();

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
   */
  AlarmTask(
      uint8_t audio_alert_pin_no,
      uint8_t led_pin_no);

  virtual ~AlarmTask();

  /**
   * Starts the alarm task.
   *
   * Parameters:
   *
   * Name                Contents
   * ------------------- ------------------------------------------------------
   * h_alarm_event_queue The queue that provides alarm request messages.
   *                     Client tasks enqueue
   */
  TaskHandle_t start(QueueHandle_t h_alarm_event_queue);
};

#endif /* ALARMTASK_H_ */
