/*
 * DeliveryStatus.h
 *
 *  Created on: Feb 19, 2023
 *      Author: Eric Mintz
 *
 * Delivery status, either awaiting delivery or milk delivered, and a message
 * for asserting same.
 */

#ifndef DELIVERYSTATUS_H_
#define DELIVERYSTATUS_H_

enum DeliveryStatus {
  DELIVERY_STATUS_CREATED,
  DELIVERY_STATUS_STARTING,
  DELIVERY_STATUS_AWAITING,
  DELIVERY_STATUS_IN_PROGRESS,
  DELIVERY_STATUS_COMPLETE,
  DELIVERY_STATUS_LID_OPEN_POST_ARRIVAL,
  DELIVERY_STATUS_SYSTEM_FAILURE,
  DELIVERY_STATUS_COUNT,  // MUST be last. State never entered.
};

struct DeliveryStatusMessage {
  DeliveryStatus status;
};

#endif /* DELIVERYSTATUS_H_ */
