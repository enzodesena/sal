/*
 MCL
 Copyright (c) 2012-18, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@gmail.com
 */

#include <assert.h>

#ifndef MCL_MCLTYPES_H
#define MCL_MCLTYPES_H

#ifndef PI
  #define PI 3.141592653589793238462643383279502884197169399375105820974944
#endif

#define ASSERT(arg) assert(arg)
#define ASSERT_WITH_MESSAGE(condition, message) ASSERT(condition && message)

#include <complex>
#include <vector>
#include <iostream>
#include <fstream>

#if _WIN32 || _WIN64
  #if _WIN64
    #define MCL_ENV64BIT 1
  #else
    #define MCL_ENV32BIT 1
  #endif
#elif __GNUC__
  #if __x86_64__ || __ppc64__
    #define MCL_ENV64BIT 1
  #else
    #define MCL_ENV32BIT 1
  #endif
#endif


#if _WIN32 || _WIN64
  #define MCL_ENVWINDOWS 1
#elif __arm__ || __aarch64__
  #define MCL_ENVARM 1
#elif __APPLE__
  #define MCL_ENVAPPLE 1
#else
  #define MCL_ENVOTHER 1
#endif

#ifdef MCL_ENVAPPLE
  #define MCL_APPLE_ACCELERATE 1
#elif MCL_ENVARM
  #define MCL_NO_ACCELERATE 1
#else // MCL_ENVWINDOWS || MCL_ENVOTHER
  #define MCL_AVX_ACCELERATE 1
#endif

// Exlude multiply and multiply-add as the compiler is able to do a better job
#define MCL_APPLE_ACCELERATE_MMA 0

#define MCL_MAX_VLA_LENGTH 5000
#define MCL_STACK_ALLOCATE(size, type) (type*)alloca((size) * sizeof(type));

namespace mcl {

#define MCL_DATA_TYPE_DOUBLE 1
typedef double Real; /**< Real type */
typedef std::complex<Real> Complex; /**< Complex type */
  
#ifdef MCL_ENV64BIT
  typedef unsigned long long UInt; /**< Unsigned int type */
  typedef long long Int; /**< Int type */
#else // If it is 32 bits or unknown then...
  typedef unsigned long UInt; /**< Unisgned int type */
  typedef long Int; /**< Int type */
#endif

class Logger {
public:
  static Logger& GetInstance() {
    static Logger instance;
    return instance;
  }
  
  enum OutputType {
    kNone,
    kCerr,
    kOutputFile
  };
  
  void LogErrorToCerr(std::string output) {
    std::cerr<<output<<std::endl;
  }
  
  void LogErrorToCerr(const char* output) {
    std::cerr<<output<<std::endl;
  }
  
  void LogError(const char* format) {
    if (output_type_ == kNone) { return; }
    
    const size_t SIZE = snprintf( NULL, 0, "%s", format);
    
    std::string output;
    output.resize(SIZE+1);
    snprintf( &(output[0]), SIZE+1, "%s", format);
    
    if (output_type_ == kCerr) {
      std::cerr<<output<<std::endl;
    } else if (output_type_ == kOutputFile) {
      log_string_.append("\n");
      log_string_.append(format);
    }
  }
  
  template< typename... argv >
  void LogError(const char* format, argv... args) {
    if (output_type_ == kNone) { return; }
    
    const size_t SIZE = snprintf( NULL, 0, format, args... );
    
    std::string output;
    output.resize(SIZE+1);
    snprintf( &(output[0]), SIZE+1, format, args... );
    
    if (output_type_ == kCerr) {
      std::cerr<<output<<std::endl;
    } else if (output_type_ == kOutputFile) {
      log_string_.append("\n");
      log_string_.append(format);
    }
  }
  
  void SetOutputType(OutputType output_type) {
    output_type_ = output_type;
  }
  
  void SetOutputFile(const std::string& log_output_file) {
    log_output_file_ = log_output_file;
  }
  
private:
  Logger() : output_type_(kCerr), log_output_file_("err.log") {}
  ~Logger() {
    if (log_string_.length() > 0) {
      std::cerr<<"Writing logger out to "<<log_output_file_<<std::endl;
      std::ofstream output_stream(log_output_file_);
      output_stream<<log_string_;
      output_stream.close();
    }
  }
  
  Logger(const Logger&);
  const Logger& operator= (const Logger&);
  OutputType output_type_;
  
  std::string log_string_;
  std::string log_output_file_;
};
  

} // namespace mcl



#endif
