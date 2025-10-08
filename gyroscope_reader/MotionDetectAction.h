/*
 * GyroscopeTask.h
 *
 *  Created on: Dec 23, 2022
 *      Author: Eric Mintz
 *
 * Read the Gyroscope and send messages. The task signals tilt when the Z
 * acceleration is <= .9 g.
 */

#ifndef GYROSCOPETASK_H_
#define GYROSCOPETASK_H_

#include "Arduino.h"
#include "dhtnew.h"
#include "Wire.h"

#include "MPU6050_light.h"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#include "MotionNotificationMessage.h"
#include "PinAssignments.h"
#include "Task.h"

/**
 * Manages the MPU6050 gyroscope, keeping its data current and detecting
 * motion. Notifies the event forwarder when the gyro's Z offset exceeds
 * a threshold or returns to the vertical.
 */
class GyroscopeTask :
    public Task {

  /**
   * Runs the update loop that refreshes the gyroscope's position data. The
   * gyroscope integrates acceleration into velocity and position.
   *
   * The update loop updates the MPU6050 readings every 2 microseconds to keep
   * its readings up to date. Update is performed on a best efforts basis.
   * Since the update task has extremely low priority, it might not
   * honor its update SLA. Assuming the SLA violation occurs infrequently,
   * this should not pose a problem.
   */
  class UpdateTask :
      Task {
    MPU6050 *gyroscope;

  public:
    UpdateTask();
    virtual ~UpdateTask();

    /**
     * Starts the gyroscope update task.
     */
    TaskHandle_t start(MPU6050 *gyroscope);

    /**
     * The update loop updates the MPU6050 readings every 2 microseconds on a
     * best efforts basis. Since it has extremely low priority, it might not
     * make its SLA.
     */
    virtual void task_loop();
  };

  UpdateTask update_task;
	QueueHandle_t h_gyro_event_queue;  // Post motion notification here.
	MPU6050 gyroscope;  // The MPU6050
	DHTNEW temperature_sensor;
	MotionNotificationMessage notification_message;

	/**
	 * The motion detection loop reads the Z acceleration, which will be 1 g
	 * when the lid is level, and alerts when it falls to or below .9 g. This
	 * happens when the lid is raised approximately 26 degrees.
	 */
	virtual void task_loop(void);

public:
	GyroscopeTask();
	virtual ~GyroscopeTask();

	/**
	 * Configure the gyroscope and bind the task to its queue handle. Note
	 * that the task sends gyroscope events to the specified queue.
	 */
	boolean begin(QueueHandle_t h_gyro_event_queue);

	/**
	 * Starts the motion detection task loop. The motion detection task detects
	 * motion and posts a notification on the gyroscope event queue.
	 */
	TaskHandle_t start_motion_detection_loop();

	/**
	 * Start the update loop. Invoke this immediately after begin() has run
	 * successfully. The update loop integrates gyroscope readings into
	 * offset angles.
	 */

	TaskHandle_t start_update_loop();
};

#endif /* GYROSCOPETASK_H_ */
