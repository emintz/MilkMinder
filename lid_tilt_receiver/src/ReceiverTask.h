/*
 * ReceiverTask.h
 *
 *  Created on: Feb 14, 2023
 *      Author: Eric Mintz
 *
 * Listens to the communications queue and drives the user interface. The
 * user interface includes the alarm (a.k.a.) beeper, LCD, and LEDS.
 */

#ifndef RECEIVERTASK_H_
#define RECEIVERTASK_H_

#include "freertos/FreeRTOS.h"

#include "Resettable.h"
#include "LidPositionReport.h"
#include "PullQueueHT.h"
#include "Task.h"

#include <esp_now.h>


class ReceiverTask :
    public Task {
  enum LidPosition {
    OPEN,
    CLOSED,
    RCV_LID_POSITION_COUNT,
  };

  Resettable * watchdog_timer;
  PullQueueHT<LidPositionReport>& lid_position_report_queue_;

  static void on_esp_now_received(
    const esp_now_recv_info* info,
      const uint8_t *received_data,
      int len);

  virtual void task_loop();

public:
  ReceiverTask(
      Resettable *watchdog_timer,
      PullQueueHT<LidPositionReport>& lid_position_report_queue);
  virtual ~ReceiverTask(void);

  static bool begin(void);

  TaskHandle_t start(void);
};

#endif /* RECEIVERTASK_H_ */
