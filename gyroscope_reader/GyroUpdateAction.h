/*
 * GyroRefresh.h
 *
 *  Created on: Oct 2, 2025
 *      Author: Eric Mintz
 *
 * Keeps the gyro current by repeatedly and frequently updating its state.
 * The gyroscope MUST be updated early and often to function properly.
 */

#ifndef GYROUPDATEACTION_H_
#define GYROUPDATEACTION_H_

#include <MPU6050_light.h>
#include <TaskAction.h>

class GyroUpdateAction : public TaskAction {
  MPU6050& gyroscope_;
public:
  /**
   * Construction
   *
   * Arguments
   *
   * Name                        Contents
   * --------------------------- ----------------------------------------------
   * gyroscope                   MPU6050 gyroscope to update.
   */
  GyroUpdateAction(MPU6050& gyroscope);
  virtual ~GyroUpdateAction();

  /**
   * Updates gyroscope data as frequently as possible, nominally once per
   * tick, load permitting
   */
  virtual void run(void) override;
};

#endif /* GYROUPDATEACTION_H_ */
