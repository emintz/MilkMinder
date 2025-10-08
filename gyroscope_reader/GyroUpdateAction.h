/*
 * GyroRefresh.h
 *
 *  Created on: Oct 2, 2025
 *      Author: Eric Mintz
 *
 * Keeps the gyro current by repeatedly and frequently updating its state.
 */

#ifndef GYROUPDATEACTION_H_
#define GYROUPDATEACTION_H_

#include <MPU6050_light.h>
#include <TaskAction.h>

class GyroUpdateAction : public TaskAction {
  MPU6050& gyroscope_;
public:
  GyroUpdateAction(MPU6050& gyroscope);
  virtual ~GyroUpdateAction();

  virtual void run(void) override;
};

#endif /* GYROUPDATEACTION_H_ */
