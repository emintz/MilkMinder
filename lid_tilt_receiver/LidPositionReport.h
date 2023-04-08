/*
 * LidPositionReport.h
 *
 *  Created on: Apr 4, 2023
 *      Author: Eric Mintz
 *
 * Message and values that report lid positions. Reports include the position
 * itself and timeouts of position changes.
 */

#ifndef LIDPOSITIONREPORT_H_
#define LIDPOSITIONREPORT_H_

/**
 * A message that reports position-related events.
 */
struct LidPositionReport {
public:

  enum PositionValue {
    LID_POS_UNCHANGED,  // No change since the last report
    LID_POS_OPEN,  // Lid has been opened
    LID_POS_CLOSED,  // Lid has been closed
    LID_POS_OPEN_TIMEOUT,  // Lid has open timeout
    LID_POS_CLOSE_TIMEOUT,  // Lid closure timeout
    LID_POS_NUMBER_OF_VALUES,  // MUST be last
  };

  PositionValue lid_position;
};

#endif /* LIDPOSITIONREPORT_H_ */
