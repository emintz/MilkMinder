/*
 * RippleAction.h
 *
 *  Created on: Oct 17, 2025
 *      Author: Eric Mintz
 */

#ifndef RIPPLEACTION_H_
#define RIPPLEACTION_H_

#include "Arduino.h"
#include "TaskAction.h"

class RippleAction : public TaskAction {
  const uint8_t *pins_;
  const size_t number_of_pins_;
  const TickType_t illumination_time_ticks_;

  void all_off(void);

public:
  RippleAction(
      const uint8_t *pins,
      const size_t number_of_pins,
      const uint16_t illumination_time_ms);
  virtual ~RippleAction() = default;

  virtual void run(void) override;
};

#endif /* RIPPLEACTION_H_ */
