/*
 * MutexLock.h
 *
 *  Created on: Apr 4, 2023
 *      Author: Eric Mintz
 *
 * A class that locks a mutex on construction and unlocks it at deletion.
 * When allocated on the stack, the class provides an exception-safe
 * lock, that is, a lock that is automatically released when it goes
 * out of scope due to an exception.
 *
 * Note: Allocate instances on the stack. DO NOT use new. Use only from
 * non-interrupt code.
 *
 * The lock supports mutexes, semaphores created by xSemaphoreCreateMutex().
 * Attempts to use it with other semaphore types will cause undefined and
 * probably undesirable behavior. Recursive locking is not supported.
 *
 */

#ifndef MUTEXLOCK_H_
#define MUTEXLOCK_H_

#include "Arduino.h"

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

class MutexLock {

  SemaphoreHandle_t h_mutex;

public:
  /**
   * Constructor, which locks the specified mutex. If the mutex is unavailable
   * when called, the constructor waits indefinitely.
   *
   * Parameters:
   *
   * Name                Description
   * ------------------- ----------------------------------------------------
   * h_mutex             Handle to a mutex type semaphore, i.e. a semaphore
   *                     created via xSemaphoreCreateMutex. Cannot be NULL.
   *                     Passing other semaphore types will cause unspecified
   *                     and probably undesirable behavior.
   */
  MutexLock(SemaphoreHandle_t h_mutex);

  /**
   * Destructor, which releases the semaphore passed to the constructor
   */
  virtual ~MutexLock();

};

#endif /* MUTEXLOCK_H_ */
