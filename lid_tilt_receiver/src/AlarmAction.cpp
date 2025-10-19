/*
 * AlarmAction.cpp
 *
 *  Created on: Feb 15, 2023
 *      Author: Eric Mintz
 */

#include <src/AlarmAction.h>
#include <stdlib.h>

#include "Arduino.h"

const struct AlarmAction::LevelAndDuration silence_levels[] = {
    { LOW, 60000 },
};
const struct AlarmAction::AlarmSignal silent_alarm = {
  1,
  silence_levels,
};


const struct AlarmAction::LevelAndDuration delivered_levels[] = {
  { HIGH, 50 },
  { LOW, 9950 },
};
const struct AlarmAction::AlarmSignal delivered_alarm = {
  2,
  delivered_levels,
};


const struct AlarmAction::LevelAndDuration disconnected_levels[] = {
  { HIGH, 500 },
  { LOW, 500 },
  { HIGH, 500 },
  { LOW, 500 },
  { HIGH, 500 },
  { LOW, 500 },
  { LOW, 7000 },
};

const struct AlarmAction::AlarmSignal disconnected_alarm = {
  7,
  disconnected_levels,
};

const struct AlarmAction::LevelAndDuration lid_open[] = {
  { HIGH, 50 },
  { LOW, 50 },
  { HIGH, 50 },
  { LOW, 50 },
  { HIGH, 50 },
  { LOW, 1250 },
};
const struct AlarmAction::AlarmSignal lid_open_signal = {
  6,
  lid_open,
};

const struct AlarmAction::LevelAndDuration lost_sync_alarm[] = {
    { HIGH, 50 },
    { LOW, 50 },
    { HIGH, 100 },
    { LOW, 50 },
};

const struct AlarmAction::AlarmSignal lost_sync_signal {
  4,
  lost_sync_alarm,
};

const struct AlarmAction::LevelAndDuration panic_alarm[] = {
  { HIGH, 950 },
  { LOW, 50 },
};

const AlarmAction::AlarmSignal panic_alarm_signal = {
  2,
  panic_alarm,
};

AlarmAction::AlarmAction(
  uint8_t audio_alert_pin_no,
  uint8_t led_pin_no,
  PullQueueHT<AlarmMessage>& alarm_event_queue) :
  audio_alert_pin_no(audio_alert_pin_no),
  led_pin_no(led_pin_no),
  alarm_event_queue_(alarm_event_queue) {
}

AlarmAction::~AlarmAction() {
}

void AlarmAction::emit_alarm(const AlarmSignal &alarm_signal) {
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

void AlarmAction::run() {
  AlarmMessage message;
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
      case ALARM_SYNC_LOST:
        emit_alarm(lost_sync_signal);
        break;
      case ALARM_EVENT_TRANSMITTER_PANIC:
        emit_alarm(panic_alarm_signal);
        break;
      }
    }
  }
}
