/*
 MCL
 Copyright (c) 18, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 */


#ifndef MCL_EXCEPTION_H
#define MCL_EXCEPTION_H

#include <iostream>
#include <string>

namespace mcl {
  
class Exception : public std::exception {
  
public:
  Exception(const char* message,
            const char* exception_code = "UNDEFINED",
            int exception_id = 0) :
  message_(message), exception_id_(exception_id),
  exception_code_(exception_code) {}
  
  ~Exception() {}
  
  virtual const char* what() const throw() {
    return message_;
  }
  
  const char* exception_code() const {
    return exception_code_;
  }
  
  int exception_id() const {
    return exception_id_;
  }
  
  static bool ExceptionTest();
private:
  const char* message_;
  const char* exception_code_;
  int exception_id_;
};
  
} // namespace mcl


#endif
