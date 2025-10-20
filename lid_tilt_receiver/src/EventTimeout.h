/*
 * EventTimeout.h
 *
 *  Created on: Oct 13, 2025
 *      Author: Eric Mintz
 *
 * Timeout action that posts debounced lid movement events for processing.
 * The milk arrival action starts the containing timer when the lid moves
 * so as to eliminate transient signals. This allows the milk deliverer to
 * open the lid repeatedly without triggering a multiple open (i.e. tamper)
 * alert.
 */

#ifndef EVENTTIMEOUT_H_
#define EVENTTIMEOUT_H_

#include "LidPositionReport.h"
#include "MutexH.h"
#include "PullQueueHT.h"
#include "VoidFunction.h"

class EventTimeout : public VoidFunction {
  PullQueueHT<LidPositionReport>& lid_position_report_queue_;
  LidPositionReport::PositionValue timeout_report;
  MutexH mutex_;

public:
  /**
   * Constructor
   *
   * Parameters                      Contents
   * ------------------------------- -------------------------------------
   * lid_position_report_queue       Carries self-posted events when time
   *                                 expires.
   */
  EventTimeout(PullQueueHT<LidPositionReport>& lid_position_report_queue);
  virtual ~EventTimeout();

  /**
   * Runs when the containing OneShotTimerH expires.
   */
  virtual void apply(void) override;

  /**
   * Initialization. Be sure to invoke this method before the
   * timeout runs. Note that this method is NOT thread safe.
   */
  bool begin();

  /**
   * Sets the position value message to send on timeout.
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

#endif /* EVENTTIMEOUT_H_ */
