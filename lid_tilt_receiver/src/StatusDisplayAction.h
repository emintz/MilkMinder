/*
 * LCDDisplayTask.h
 *
 *  Created on: Feb 9, 2023
 *      Author: Eric Mintz
 *
 * Displays the system status on a the liquid crystal display.
 */

#ifndef LCDDISPLAYTASK_H_
#define LCDDISPLAYTASK_H_

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "TaskAction.h"

#include "DisplayMessage.h"
#include "LiquidCrystal_I2C.h"
#include "PullQueueHT.h"
#include "TimeTask.h"


class StatusDisplayAction : public TaskAction {
  LiquidCrystal_I2C& display_;
  TimeTask *time_task_;
  PullQueueHT<DisplayMessage>& display_command_queue_;

  /**
   * Display "Network Connected" status
   */
  void connected();

  /**
   * Display "Network Disconnected" status
   */
  void disconnected();

  /**
   * Task run loop
   */
  virtual void run() override;

public:
  /**
   * Constructor
   *
   * Parameters            Contents
   * --------------------- -------------------------------------
   * display               Low level display instance
   * time_task             Timer task, provides delivery time
   * display_command_queue Provides the commands that drive the
   *                       display
   */
  StatusDisplayAction(
      LiquidCrystal_I2C& display,
      TimeTask *time_task,
      PullQueueHT<DisplayMessage>& display_command_queue);
  virtual ~StatusDisplayAction();
};

#endif /* LCDDISPLAYTASK_H_ */
