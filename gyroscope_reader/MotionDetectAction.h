/*
 * GyroscopeTask.h
 *
 *  Created on: Dec 23, 2022
 *      Author: Eric Mintz
 *
 * Read the Gyroscope and send messages. The task signals tilt when the Z
 * acceleration is <= .9 g.
 */

#ifndef MOTIONDETECTACTION_H_
#define MOTIONDETECTACTION_H_

#include "Arduino.h"
#include "dhtnew.h"
#include "MPU6050_light.h"
#include "PullQueueHT.h"
#include "Wire.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "MotionNotificationMessage.h"
#include "PinAssignments.h"
#include "Task.h"
#include "TaskAction.h"

#define DEGREES_TO_RADIANS (PI / 180.0)
#define RADIANS_TO_DEGREES (180.0 / PI)
#define INCLINATION_THRESHOLD (PI / 6)

/**
 * Monitors the MPU6050 gyroscope to detect motion. Notifies the event
 * forwarder when the gyro's Z offset exceeds the lid open threshold
 * or returns to the horizontal.
 */
class MotionDetectAction : public TaskAction {
	MPU6050& gyroscope_;
	DHTNEW temperature_sensor_;
	MotionNotificationMessage notification_message_;

	/**
	 * The motion detection loop reads the Z acceleration, which will be 1 g
	 * when the lid is level, and alerts when it exceeds INCLINATION_THRESHOLD.
	 */
//	virtual void task_loop(void);

    PullQueueHT<MotionNotificationMessage>& gyroscope_event_queue_;

public:
	MotionDetectAction(
		PullQueueHT<MotionNotificationMessage>& gyroscope_event_queue,
		MPU6050 &gyroscope);
	virtual ~MotionDetectAction();

	/**
	 * Configure the gyroscope and bind the task to its queue handle. Note
	 * that the task sends gyroscope events to the specified queue.
	 */
	boolean begin();


	/**
	 * The motion detection loop reads the tilt angle, which will be 0
	 * when the lid is level, and alerts when exceeds  This
	 * happens when the tilt angle off the horizontal exceeds
	 * INCLINATION_THRESHOLD. Note that the closed position is 0 degrees.
	 */
	virtual void run(void) override;

	/**
	 * Starts the motion detection task loop. The motion detection task detects
	 * motion and posts a notification on the gyroscope event queue.
	 */
//	TaskHandle_t start_motion_detection_loop();
};

#endif /* MOTIONDETECTACTION_H_ */
