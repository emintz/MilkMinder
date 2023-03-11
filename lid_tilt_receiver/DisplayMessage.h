/*
 * DisplayMessage.h
 *
 *  Created on: Feb 9, 2023
 *      Author: Eric Mintz
 *
 * LCD Display Commands. Each command causes the LCD Display Task to write
 * a predefined message to the display. Please see LCDDisplayTask.cpp for
 * details.
 */

#ifndef DISPLAYMESSAGE_H_
#define DISPLAYMESSAGE_H_

#include "Arduino.h"

#define MAX_LCD_TEXT_LENGTH 16

enum DisplayCommand {
  LCD_CLEAR,              // Clear the display
  LCD_CONNECTED,          // Connected to the receiver
  LCD_DELIVERED,          // Display delivered message
  LCD_DISCONNECTED,       // Sender signal lost
  LCD_ELAPSED,            // Elapsed time since delivery
  LCD_INIT,               // Display initialization message
  LCD_NOOP,               // Do nothing
  LCD_RUN,                // Display run message
  LCD_TIME_OF_DAY,        // Display time of day
  LCD_TRANSMITTER_PANIC,  // Transmitter failure, e.g. gyroscope down
};

struct DisplayMessage {
  DisplayCommand command;
  char text[MAX_LCD_TEXT_LENGTH+1];  // NULL terminated string
};

#endif /* DISPLAYMESSAGE_H_ */
