/*
 * LCDDisplayTask.h
 *
 *  Created on: Feb 9, 2023
 *      Author: Eric Mintz
 *
 * Displays delivery and network status on a 2 x 16 liquid crystal display.
 */

#ifndef LCDDISPLAYTASK_H_
#define LCDDISPLAYTASK_H_

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "DisplayMessage.h"
#include "LiquidCrystal_I2C.h"
#include "PullQueueHT.h"
#include "Task.h"
#include "TimeTask.h"


class LCDDisplayTask : public Task {
  LiquidCrystal_I2C& display;
  TimeTask *time_task;
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
  virtual void task_loop();

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
  LCDDisplayTask(
      LiquidCrystal_I2C& display,
      TimeTask *time_task,
      PullQueueHT<DisplayMessage>& display_command_queue);
  virtual ~LCDDisplayTask();

  /**
   * Starts the display task.
   */
  TaskHandle_t start();
};

#endif /* LCDDISPLAYTASK_H_ */
