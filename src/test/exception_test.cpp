/*
 MCL
 Copyright (c) 18, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 */


#include "exception.h"
#include <cassert>

namespace mcl {
  
bool Exception::ExceptionTest() {

  bool exception_triggered(false);
  
  try {
    throw(Exception("Hello", "TEST_EXCEPTION", 1234));
  } catch (Exception& exception) {
    exception_triggered = true;
    assert(exception.exception_id() == 1234);
  }
  
  assert(exception_triggered);
  
  return true;
}
  
} // namespace mcl

