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
  { HIGH, 50 },
  { LOW, 9950 },
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
  { HIGH, 50 },
  { LOW, 50 },
  { HIGH, 50 },
  { LOW, 50 },
  { HIGH, 50 },
  { LOW, 1250 },
};
const struct AlarmTask::AlarmSignal lid_open_signal = {
  6,
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
  uint8_t led_pin_no,
  PullQueueHT<AlarmTask::AlarmTaskMessage>& alarm_event_queue) :
  Task(
      "alarm",
      2048,
      5),
  audio_alert_pin_no(audio_alert_pin_no),
  led_pin_no(led_pin_no),
  alarm_event_queue_(alarm_event_queue) {
}

AlarmTask::~AlarmTask() {
}

void AlarmTask::emit_alarm(const AlarmSignal &alarm_signal) {
  while (!alarm_event_queue_.waiting_message_count()) {
    for (
        size_t i = 0;
        !alarm_event_queue_.waiting_message_count()
            && i < alarm_signal.level_count;
        ++i) {
      const LevelAndDuration *level_and_duration = alarm_signal.level + i;
      digitalWrite(audio_alert_pin_no, level_and_duration->level);
      digitalWrite(led_pin_no, level_and_duration->level);
      vTaskDelay(level_and_duration->duration_ms);
    }
  }
}

void AlarmTask::task_loop() {
  AlarmTaskMessage message;
  for (;;) {
    memset(&message, 0, sizeof(message));
    if (alarm_event_queue_.pull_message(&message)) {
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

TaskHandle_t AlarmTask::start() {
  return create_and_start_task();
}
