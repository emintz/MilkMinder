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
#include "freertos/queue.h"
#include "freertos/task.h"

#include "LiquidCrystal_I2C.h"
#include "Task.h"
#include "TimeTask.h"


class LCDDisplayTask :
    public Task {
  LiquidCrystal_I2C display;
  QueueHandle_t h_display_command_queue;
  TimeTask *time_task;

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
   * Parameters     Contents
   * -------------- -------------------------------------
   * display        Low level display instance
   * time_task      Timer task, provides delivery time
   */
  LCDDisplayTask(
      LiquidCrystal_I2C display,
      TimeTask *time_task);
  virtual ~LCDDisplayTask();

  /**
   *
   *
   * Parameters         Contents
   * -----------------  -------------------------------------
   * h_display_command  LCD command queue. The queue carries
   *                    commands to write information to the LCD.
   */
  TaskHandle_t start(QueueHandle_t h_display_command);
};

#endif /* LCDDISPLAYTASK_H_ */
