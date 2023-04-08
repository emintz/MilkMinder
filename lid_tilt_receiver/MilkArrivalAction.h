/*
 * MilkArrivalAction.h
 *
 *  Created on: Apr 4, 2023
 *      Author: Eric Mintz
 *
 * The timeout action for the milk arrival task. The action sends a specified
 * LidPositionReport
 */

#ifndef MILKARRIVALACTION_H_
#define MILKARRIVALACTION_H_

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"

#include "Action.h"

#include "LidPositionReport.h"

class MilkArrivalAction : public Action {

  QueueHandle_t h_lid_position_report_queue;
  SemaphoreHandle_t h_mutex;
  LidPositionReport::PositionValue timeout_report;
  StaticSemaphore_t semaphore_buffer;

public:
  MilkArrivalAction();
  virtual ~MilkArrivalAction();

  /**
   * Initialize the action. Be sure to invoke this method before the
   * action runs.
   *
   * Parameters
   *
   * Name                          Description
   * ----------------------------- --------------------------------------------
   *  h_lid_position_report_queue  Queue that transmits timeout signals. Must
   *                               be a valid queue handle and cannot be NULL.
   */
  void begin(QueueHandle_t h_lid_position_report_queue);

  /**
   * Runs the action, which enqueues the currently configured lid position
   * report value to the lid position report queue. The queue is set in begin(),
   * and the report value is set in set_timeout_report.
   */
  virtual void run();

  /**
   * Sets the position value to send on timeout.
   *
   * Parameters
   *
   * Name               Description
   * ------------------ ----------------------------------------------------
   * timeout_report     The lid position report value to send when the timer
   *                    expires (i.e, when the countdown completes)
   */
  void set_timeout_report(LidPositionReport::PositionValue timeout_report);
};

#endif /* MILKARRIVALACTION_H_ */
