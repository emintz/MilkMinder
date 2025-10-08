/*
 * GyroRefresh.cpp
 *
 *  Created on: Oct 2, 2025
 *      Author: Eric Mintz
 */

#include <GyroUpdateAction.h>

GyroUpdateAction::GyroUpdateAction(MPU6050& gyroscope) :
  gyroscope_(gyroscope) {

}

GyroUpdateAction::~GyroUpdateAction() {
}

void GyroUpdateAction::run(void) {
  for(;;) {
    gyroscope_.update();
    vTaskDelay(1);  // Shortest possible delay.
  }
}
