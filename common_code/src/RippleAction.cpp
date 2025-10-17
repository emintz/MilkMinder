/*
 * RippleAction.cpp
 *
 *  Created on: Oct 17, 2025
 *      Author: Eric Mintz
 */

#include "RippleAction.h"

RippleAction::RippleAction(
    const uint8_t *pins,
    const size_t number_of_pins,
    const uint16_t illumination_time_ms) :
      pins_(pins),
      number_of_pins_(number_of_pins),
      illumination_time_ticks_(pdMS_TO_TICKS(illumination_time_ms)){
}

void RippleAction::all_off(void) {
  const uint8_t *current_pin = pins_;
  for (size_t pin_no = 0; pin_no < number_of_pins_; ++pin_no) {
    digitalWrite(((unsigned char) *current_pin++), LOW);
  }
}

void RippleAction::run(void) {
  all_off();
  for (;;) {
    const uint8_t *current_pin = pins_;
    for (size_t pin_no = 0; pin_no < number_of_pins_; ++pin_no) {
      all_off();
      digitalWrite(*current_pin++, HIGH);
      vTaskDelay(illumination_time_ticks_);
    }
  }
}
