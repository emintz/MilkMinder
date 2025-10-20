/*
 * MilkArrivalAction.h
 *
 *  Created on: Apr 4, 2023
 *      Author: Eric Mintz
 *
 * Task action that tracks and processes milk arrival data.
 *
 * This is the heart of the system, a dispatcher that receives
 * lid position events from the lid position report queue, processes
 * them, and dispatches events to subsystem management tasks.
 * Messages arrive from the ESP-Now receiver and commands
 * are sent to:
 *
 *   1. The LCD driver that manages the 16 x 2 status display
 *   2. LED driver that manages the 5 status LEDs
 *   3. The alarm task that flashes an LED and sounds a tone.
 */

#ifndef MILKARRIVALTASK_H_
#define MILKARRIVALTASK_H_

#include "Arduino.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "AlarmAction.h"
#include "DeliveryLEDIlluminationStatus.h"
#include "EventTimeout.h"
#include "LidPositionReport.h"
#include "OneShotTimerH.h"
#include "PullQueueHT.h"
#include "TaskAction.h"
#include "TimeAction.h"

class MilkArrivalAction : public TaskAction {

  enum ArrivalState {
    MILK_ARRIVAL_CRREATED,  // Creation state
    MILK_ARRIVAL_WAITING_FOR_ARRIVAL,
    MILK_ARRIVAL_SUSPECT_DELIVERY_HAS_BEGUN,
    MILK_ARRIVAL_CONFIRMED_DELEVERY_HAS_BEGUN,
    MILK_ARRIVAL_SUSPECT_DELIVERY_IS_COMPLETE,
    MILK_ARRIVAL_CONFIRMED_DELIVERY_IS_COMPLETE,
    MILK_ARRIVAL_SUSPECT_TAMPERING,
    MILK_ARRIVAL_CONFIRMED_TAMPERING,
    MILK_ARRIVAL_SYNC_LOST,
    MILK_ARRIVAL_NUMBER_OF_STATES,
  };

  static ArrivalState STATE_TRANSITION_TABLE
      [MILK_ARRIVAL_NUMBER_OF_STATES]
      [LidPositionReport::LID_POS_NUMBER_OF_VALUES];

  TimeAction *time_action_;
  PullQueueHT<AlarmMessage>& alarm_event_queue_;
  PullQueueHT<LedIlluminationMessage>& delivery_led_illumination_queue_;
  PullQueueHT<DisplayMessage>& display_command_queue_;
  PullQueueHT<LidPositionReport>& lid_position_report_queue_;

  ArrivalState state_;
  EventTimeout on_timeout_;
  OneShotTimerH event_timer_;

  /**
   * Stops the countdown and resets the enqueued message to
   * "lid has not moved"
   */
  void halt_countdown(void);

  /**
   * Enqueues a "lid is open" message and starts the transmission
   * countdown.
   */
  void lid_is_open(void);

  /**
   * Silence any running alarm.
   */
  void quiesce(void);

  /**
   * Starts the state change timeout
   */
  void start_countdown(
      TickType_t timeout,
      LidPositionReport::PositionValue notification_on_expiration);

public:
  /**
   * Constructor
   *
   * Parameters:
   * ----------
   *
   * Name                  Contents
   * --------------------- ----------------------------------------------------
   * time_action                     Tracks the current time and sends it
   *                                 to the LCD task for display
   * alarm_event_queue               Output queue for publishing alarms
   * delivery_led_illumination_queue Output queue for milk delivery LED
   *                                 illumination commands
   * lid_position_report_queue       Output queue for lid position reports
   */
  MilkArrivalAction(
      TimeAction * time_action,
      PullQueueHT<AlarmMessage>& alarm_event_queue,
      PullQueueHT<LedIlluminationMessage>& delivery_led_illumination_queue,
      PullQueueHT<DisplayMessage>& display_command_queue,
      PullQueueHT<LidPositionReport>& lid_position_report_queue);
  virtual ~MilkArrivalAction();

  /**
   * Task logic
   */
  virtual void run(void) override;
};

#endif /* MILKARRIVALTASK_H_ */
