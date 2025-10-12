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

#include "MutexH.h"

#include "Action.h"
#include "LidPositionReport.h"
#include "PullQueueHT.h"

class MilkArrivalAction : public Action {
  PullQueueHT<LidPositionReport>& lid_position_report_queue_;
  LidPositionReport::PositionValue timeout_report;
  MutexH mutex_;

public:
  MilkArrivalAction(PullQueueHT<LidPositionReport>& lid_position_report_queue);
  virtual ~MilkArrivalAction();

  /**
   * Initialize the action. Be sure to invoke this method before the
   * action runs. Note that this method is NOT thread safe.
   */
  void begin();

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
