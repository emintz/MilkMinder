/*
 * ConnectionStatus.h
 *
 *  Created on: Feb 20, 2023
 *      Author: Eric Mintz
 *
 * Connection status to the sender, that is, the gyroscope reader.
 */

#ifndef CONNECTIONSTATUS_H_
#define CONNECTIONSTATUS_H_

enum ConnectionStatus {
  CONNECTION_STATUS_DOWN,  // WIFI disconnected
  CONNECTION_STATUS_UP,  // WIFI connected
  CONNECTION_STATUS_SENDER_FAILED,  // Unrecoverable error on the sender
  CONNECTION_STATUS_COUNT,   // MUST be last: status count & ignore event signal.
};

struct ConnectionStatusMessage {
  ConnectionStatus status;
};

#endif /* CONNECTIONSTATUS_H_ */
