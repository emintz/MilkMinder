/*
 * DeliveryLEDIlluminationStatus.h
 *
 *  Created on: Feb 20, 2023
 *      Author: Eric Mintz
 *
 * Delivery LED illumination status, how the delivery LED should be illuminated
 */

#ifndef DELIVERYLEDILLUMINATIONSTATUS_H_
#define DELIVERYLEDILLUMINATIONSTATUS_H_

enum LedIllumination {
  DELIVERY_LED_OFF,  // LED turned off
  DELIVERY_LED_BLINK,  // LED blinking
  DELIVERY_LED_ON,  // LED turned on
};

struct LedIlluminationMessage {
  LedIllumination illumination;
};

#endif /* DELIVERYLEDILLUMINATIONSTATUS_H_ */
