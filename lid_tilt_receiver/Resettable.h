/*
 * Resettable.h
 *
 *  Created on: May 21, 2023
 *      Author: Eric Mintz
 */

#ifndef RESETTABLE_H_
#define RESETTABLE_H_

class Resettable {
public:
  Resettable();
  virtual ~Resettable();

  virtual void reset(void) = 0;
};

#endif /* RESETTABLE_H_ */
