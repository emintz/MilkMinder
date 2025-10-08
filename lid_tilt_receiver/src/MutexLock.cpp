/*
 * MutexLock.cpp
 *
 *  Created on: Apr 4, 2023
 *      Author: Eric Mintz
 */

#include "MutexLock.h"

MutexLock::MutexLock(SemaphoreHandle_t h_mutex) :
    h_mutex(h_mutex) {
  BaseType_t status = xSemaphoreTake(this->h_mutex, portMAX_DELAY);
}

MutexLock::~MutexLock() {
 xSemaphoreGive(h_mutex);
 h_mutex = NULL;
}
