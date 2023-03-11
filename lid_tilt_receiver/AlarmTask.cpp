/*
 * AlarmTask.cpp
 *
 *  Created on: Feb 15, 2023
 *      Author: Eric Mintz
 */

#include "AlarmTask.h"

#include <stdlib.h>

#include "Arduino.h"

const struct AlarmTask::LevelAndDuration silence_levels[] = {
    { LOW, 60000 },
};
const struct AlarmTask::AlarmSignal silent_alarm = {
    1,
    silence_levels,
};


const struct AlarmTask::LevelAndDuration delivered_levels[] = {
    { HIGH, 100 },
    { LOW, 9900 },
};
const struct AlarmTask::AlarmSignal delivered_alarm = {
    2,
    delivered_levels,
};


const struct AlarmTask::LevelAndDuration disconnected_levels[] = {
    { HIGH, 500 },
    { LOW, 500 },
    { HIGH, 500 },
    { LOW, 500 },
    { HIGH, 500 },
    { LOW, 500 },
    { LOW, 7000 },
};
const struct AlarmTask::AlarmSignal disconnected_alarm = {
    7,
    disconnected_levels,
};

const struct AlarmTask::LevelAndDuration lid_open[] = {
    { HIGH, 100 },
    { LOW, 100 },
};
const struct AlarmTask::AlarmSignal lid_open_signal = {
    2,
    lid_open,
};

const struct AlarmTask::LevelAndDuration panic_alarm[] = {
    { HIGH, 950 },
    { LOW, 50 },
};

const AlarmTask::AlarmSignal panic_alarm_signal = {
    2,
    panic_alarm,
};

AlarmTask::AlarmTask(
    uint8_t audio_alert_pin_no,
    uint8_t led_pin_no) :
    Task(
        "alarm",
        2048,
        5),
    h_alarm_event_queue(NULL),
    audio_alert_pin_no(audio_alert_pin_no),
    led_pin_no(led_pin_no) {
}

AlarmTask::~AlarmTask() {
}

void AlarmTask::emit_alarm(const AlarmSignal &alarm_signal) {
  AlarmTaskMessage message;
  while (!uxQueueMessagesWaiting(h_alarm_event_queue)) {
    for (
        size_t i = 0;
        !uxQueueMessagesWaiting(h_alarm_event_queue)
            && i < alarm_signal.level_count;
        ++i) {
      const LevelAndDuration *level_and_duration = alarm_signal.level + i;
      digitalWrite(audio_alert_pin_no, level_and_duration->level);
      digitalWrite(led_pin_no, level_and_duration->level);
      xQueuePeek(
          h_alarm_event_queue,
          &message,
          pdMS_TO_TICKS(level_and_duration->duration_ms));
    }
  }
}

void AlarmTask::task_loop() {
  AlarmTaskMessage message;
  for (;;) {
    memset(&message, 0, sizeof(message));
    if (xQueueReceive(h_alarm_event_queue, &message, 0) ) {
      switch (message.event) {
      case ALARM_EVENT_CONNECTED:
        emit_alarm(silent_alarm);
        break;
      case ALARM_EVENT_DISCONNECTED:
        emit_alarm(disconnected_alarm);
        break;
      case ALARM_EVENT_DELIVERED:
        emit_alarm(delivered_alarm);
        break;
      case ALARM_EVENT_LID_OPEN:
        emit_alarm(lid_open_signal);
        break;
      case ALARM_EVENT_TRANSMITTER_PANIC:
        emit_alarm(panic_alarm_signal);
        break;
      }
    }
  }
}

TaskHandle_t AlarmTask::start(QueueHandle_t h_alarm_event_queue) {
  this->h_alarm_event_queue = h_alarm_event_queue;
  return create_and_start_task();
}
