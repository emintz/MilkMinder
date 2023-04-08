/*
 * Action.h
 *
 *  Created on: Mar 28, 2023
 *      Author: Eric Mintz
 *
 * Base action class. Implementations can do anything and generate
 * arbitrary side effects.
 */

#ifndef ACTION_H_
#define ACTION_H_

class Action {
public:
  /**
   * Place holder constructor that does nothing.
   */
  Action();

  /**
   * Place holder destructor that only ensures that implementations have
   * a virtual function table.
   */
  virtual ~Action();

  /**
   * Runs the action, which may perform arbitrary logic and produce arbitrary
   * effects.
   */
  virtual void run(void) = 0;
};

#endif /* ACTION_H_ */
