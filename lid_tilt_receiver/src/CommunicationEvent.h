/*
 * CommunicationEvent.h
 *
 *  Created on: Feb 10, 2023
 *      Author: Eric Mintz
 *
 * Communication-related event. The event represents both communication
 * status and incoming events.
 */

#ifndef COMMUNICATIONEVENT_H_
#define COMMUNICATIONEVENT_H_

#include "MotionNotificationMessage.h"

struct CommunicationEvent {
  MotionNotificationMessage event_type;
};

#endif /* COMMUNICATIONEVENT_H_ */
