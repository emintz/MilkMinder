/*
 * BlinkTask.h
 *
 *  Created on: Dec 25, 2022
 *      Author: Eric Mintz
 *
 * Task that blinks an LED. When active, the task flashes the specified LED
 * the specified number of times then waits for the specified wait time.
 */

#ifndef BLINKTASK_H_
#define BLINKTASK_H_

#include "Arduino.h"

#include "Task.h"

class BlinkTask :
    public Task {
	uint16_t led_pin;
	uint16_t number_of_flashes;
	TickType_t inter_flash_wait_ticks;
	TickType_t inter_group_wait_ticks;
	TaskHandle_t h_this_task;

	/**
	 * The task loop.
	 */
	virtual void task_loop();

public:
	/**
	 * Initialize the task and set the default blink characteristics.
	 *
	 * Parameters            Contents
	 * --------------------- ------------------------------------------------
	 * led_pin               Powers the LED to blink
	 * number_of_flashes     The number of times to flash.
	 * inter_flash_wait_ms   The blink rate, the "on" and "off" times in
	 *                       milliseconds. Note that the on and off times are
     *                       always equal.
	 * inter_group_wait_ms   The wait time between between flash groups in
	 *                       milliseconds
	 */
	BlinkTask(
		const char *name,
		const uint16_t led_pin,
		const uint16_t number_of_flashes,
		const uint16_t inter_flash_wait_ms,
		const uint16_t inter_group_wait_ms);
	virtual ~BlinkTask();

	/**
	 * Create and start the task that runs the blink loop. The task comes up
	 * suspended. Call resume() to start the blink.
	 */
	TaskHandle_t start_blink_loop(const char *name);

	/**
	 * Stop the blink.
	 */
	void suspend();

	/**
	 * Start the blink using the existing settings.
	 */
	void resume();

	/**
	 * Starts the blink using the provided settings.
	 *
	 * Parameters            Contents
	 * --------------------- ------------------------------------------------
	 * number_of_flashes     The number of times to flash.
	 * inter_flash_wait_ms   The blink rate, the "on" and "off" times in
	 *                       milliseconds. Note that the on and off times are
     *                       always equal.
	 * inter_group_wait_ms   The wait time between between flash groups in
	 *                       milliseconds
	 */
	void resume(
		uint16_t number_of_flashes,
		TickType_t inter_flash_wait_ticks,
		TickType_t inter_group_wait_ticks);
};

#endif /* BLINKTASK_H_ */
