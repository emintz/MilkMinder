/*
 * PinAssignments.h
 *
 *  Created on: Dec 23, 2022
 *      Author: Eric Mintz
 *
 * GPIO Assignments, how pins are used
 */

#ifndef PINASSIGNMENTS_H_
#define PINASSIGNMENTS_H_

#define SYSTEM_IS_LIVE_LED_PIN 2

#define BUILTIN_LED_PIN 2  // On-board LED.
#define RED_LED_PIN 13  // Red indicator LED
#define YELLOW_LED_PIN 14  // Yellow indicator LED
#define GREEN_LED_PIN  15  // Green indicator LED
#define BLUE_LED_PIN 16  // Blue indicator LED
#define IO_INTERRUPT_PIN 17  // TODO: change in sender, too.
#define TEMPERATURE_AND_HUMIDITY_PIN 25
#define ALARM_PIN 33  // HIGH sounds the alarm

#endif /* PINASSIGNMENTS_H_ */
