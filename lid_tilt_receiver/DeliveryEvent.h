/*
 * DeliveryEvent.h
 *
 *  Created on: Feb 23, 2023
 *      Author: Eric Mintz
 *
 * Events that affect milk delivery state.
 */

#ifndef DELIVERYEVENT_H_
#define DELIVERYEVENT_H_

enum DeliveryEvent {
  DELIVERY_EVENT_STARTING,
  DELIVERY_EVENT_LISTENING,
  DELIVERY_EVENT_LID_OPENED,
  DELIVERY_EVENT_LID_CLOSED,
  DELIVERY_EVENT_TRANSMITTER_FAILED,
  DELIVERY_STATUS_INTERNAL_ERROR,
  DELIVERY_EVENT_COUNT,  // MUST be last.
};

struct DeliveryEventMessage {
  DeliveryEvent event;
};

#endif /* DELIVERYEVENT_H_ */
