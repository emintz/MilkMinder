/*
 * Resettable.h
 *
 *  Created on: May 21, 2023
 *      Author: Eric Mintz
 *
 * Interface implemented by classes that can be reset.
 */

#ifndef RESETTABLE_H_
#define RESETTABLE_H_

class Resettable {
public:
  virtual ~Resettable() = default;

  virtual void reset(void) = 0;
};

#endif /* RESETTABLE_H_ */
