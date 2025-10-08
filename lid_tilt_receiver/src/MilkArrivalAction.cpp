/*
 * MilkArrivalAction.cpp
 *
 *  Created on: Apr 4, 2023
 *      Author: Eric Mintz
 */

#include "MilkArrivalAction.h"

#include "string.h"

#include "MutexLock.h"

MilkArrivalAction::MilkArrivalAction() :
    Action(),
    h_lid_position_report_queue(NULL),
    timeout_report(LidPositionReport::LID_POS_UNCHANGED) {
  memset(&semaphore_buffer, 0, sizeof(semaphore_buffer));
  h_mutex = xSemaphoreCreateMutexStatic(&semaphore_buffer);
}

MilkArrivalAction::~MilkArrivalAction() {
}

void MilkArrivalAction::begin(QueueHandle_t h_lid_position_report_queue) {
  MutexLock lock(h_mutex);
  this->h_lid_position_report_queue = h_lid_position_report_queue;
}

void MilkArrivalAction::run() {
  LidPositionReport report;
  report.lid_position = timeout_report;
  MutexLock lock(h_mutex);
  xQueueSendToBack(h_lid_position_report_queue, &report, pdMS_TO_TICKS(10));
}

void MilkArrivalAction::set_timeout_report(
    LidPositionReport::PositionValue timeout_report) {
  MutexLock lock(h_mutex);
  this->timeout_report = timeout_report;
}
