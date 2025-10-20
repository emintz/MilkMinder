/*
 * ReceiveAction.h
 *
 *  Created on: Feb 14, 2023
 *      Author: Eric Mintz
 *
 * Listens to the communications queue and drives the user interface. The
 * user interface includes the alarm (a.k.a.) beeper, LCD, and LEDS.
 */

#ifndef RECEIVEACTION_H_
#define RECEIVEACTION_H_

#include "freertos/FreeRTOS.h"

#include "Resettable.h"
#include "LidPositionReport.h"
#include "PullQueueHT.h"
#include "TaskAction.h"

#include <esp_now.h>


class ReceiveAction : public TaskAction {
  enum LidPosition {
    OPEN,
    CLOSED,
    RCV_LID_POSITION_COUNT,
  };

  Resettable * watchdog_timer_;
  PullQueueHT<LidPositionReport>& lid_position_report_queue_;
  uint16_t lid_open_count_;

  static void on_esp_now_received(
    const esp_now_recv_info* info,
      const uint8_t *received_data,
      int len);

  virtual void run(void) override;

public:
  ReceiveAction(
      Resettable *watchdog_timer,
      PullQueueHT<LidPositionReport>& lid_position_report_queue);
  virtual ~ReceiveAction(void);

  static bool begin(void);
};

#endif /* RECEIVEACTION_H_ */
