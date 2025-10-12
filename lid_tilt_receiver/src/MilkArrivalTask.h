/*
 * MilkArrivalTask.h
 *
 *  Created on: Apr 4, 2023
 *      Author: Eric Mintz
 *
 * Task that tracks milk arrival.
 */

#ifndef MILKARRIVALTASK_H_
#define MILKARRIVALTASK_H_

#include "Arduino.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "Action.h"
#include "AlarmTask.h"
#include "DeliveryLEDIlluminationStatus.h"
#include "LidPositionReport.h"
#include "MilkArrivalAction.h"
#include "OneShotTimerWithAction.h"
#include "PullQueueHT.h"
#include "Task.h"
#include "TimeTask.h"

class MilkArrivalTask : public Task {

  enum ArrivalState {
    MILK_ARRIVAL_CRREATED,  // Creation state
    MILK_ARRIVAL_WAITING_FOR_ARRIVAL,
    MILK_ARRIVAL_SUSPECT_DELIVERY_HAS_BEGUN,
    MILK_ARRIVAL_CONFIRMED_DELEVERY_HAS_BEGUN,
    MILK_ARRIVAL_SUSPECT_DELIVERY_IS_COMPLETE,
    MILK_ARRIVAL_CONFIRMED_DELIVERY_IS_COMPLETE,
    MILK_ARRIVAL_SUSPECT_TAMPERING,
    MILK_ARRIVAL_CONFIRMED_TAMPERING,
    MILK_ARRIVAL_NUMBER_OF_STATES,
  };

  static ArrivalState STATE_TRANSITION_TABLE
      [MILK_ARRIVAL_NUMBER_OF_STATES]
      [LidPositionReport::LID_POS_NUMBER_OF_VALUES];

  TimeTask *time_task;
  PullQueueHT<AlarmTask::AlarmTaskMessage>& alarm_event_queue_;
  PullQueueHT<LedIlluminationMessage>& delivery_led_illumination_queue_;
  PullQueueHT<DisplayMessage>& display_command_queue_;
  PullQueueHT<LidPositionReport>& lid_position_report_queue_;

  QueueHandle_t h_lid_position_report_queue;
  ArrivalState state;
  MilkArrivalAction timeout_action;
  OneShotTimerWithAction timer;


  void halt_countdown(void);

  void lid_is_open(void);

  void quiesce(void);

  void start_countdown(
      TickType_t timeout,
      LidPositionReport::PositionValue notification_on_expiration);

public:
  MilkArrivalTask(
      TimeTask * time_task,
      PullQueueHT<AlarmTask::AlarmTaskMessage>& alarm_event_queue,
      PullQueueHT<LedIlluminationMessage>& delivery_led_illumination_queue,
      PullQueueHT<DisplayMessage>& display_command_queue,
      PullQueueHT<LidPositionReport>& lid_position_report_queue);
  virtual ~MilkArrivalTask();

  TaskHandle_t start();
  virtual void task_loop();
};

#endif /* MILKARRIVALTASK_H_ */
