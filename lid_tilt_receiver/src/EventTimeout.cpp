/*
 * EventTimeout.cpp
 *
 *  Created on: Oct 13, 2025
 *      Author: Eric Mintz
 */

#include "EventTimeout.h"

#include "MutexH.h"

EventTimeout::EventTimeout(
    PullQueueHT<LidPositionReport>& lid_position_report_queue) :
      lid_position_report_queue_(lid_position_report_queue),
      timeout_report(LidPositionReport::LID_POS_UNCHANGED) {
}

EventTimeout::~EventTimeout() {
}

void EventTimeout::apply(void) {
  LidPositionReport report;
  report.lid_position = timeout_report;
  MutexLock lock(mutex_);
  lid_position_report_queue_.send_message(&report, pdMS_TO_TICKS(10));
}

bool EventTimeout::begin(void) {
  return mutex_.begin();
}

void EventTimeout::set_timeout_report(
    LidPositionReport::PositionValue timeout_report) {
  MutexLock lock(mutex_);
  this->timeout_report = timeout_report;
}
