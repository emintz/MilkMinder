/*
 * MilkArrivalAction.cpp
 *
 *  Created on: Apr 4, 2023
 *      Author: Eric Mintz
 */

#include "MilkArrivalAction.h"

#include "string.h"

#include "MutexLock.h"

MilkArrivalAction::MilkArrivalAction(
    PullQueueHT<LidPositionReport>& lid_position_report_queue) :
      Action(),
      lid_position_report_queue_(lid_position_report_queue),
      timeout_report(LidPositionReport::LID_POS_UNCHANGED) {
}

MilkArrivalAction::~MilkArrivalAction() {
}

void MilkArrivalAction::begin() {
  mutex_.begin();
}

void MilkArrivalAction::run() {
  LidPositionReport report;
  report.lid_position = timeout_report;
  MutexLock lock(mutex_);
  lid_position_report_queue_.send_message(&report, pdMS_TO_TICKS(10));
}

void MilkArrivalAction::set_timeout_report(
    LidPositionReport::PositionValue timeout_report) {
  MutexLock lock(mutex_);
  this->timeout_report = timeout_report;
}
