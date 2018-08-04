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

#define throw_line(arg) throw(mcl::Exception(std::string("Error at line n. ") + \
      std::string(std::to_string(__LINE__)) + std::string(" of file ") + \
      std::string(__FILE__) + std::string(" ") + arg))
// #define throw_line(arg) throw std::runtime_error(std::string("Error at line n. ") +std::string(std::to_string(__LINE__)) + std::string(" of file ") + std::string(__FILE__));

#include <complex>
#include <vector>
#include <iostream>

#if _WIN32 || _WIN64
  #if _WIN64
    #define ENV64BIT
  #else
    #define ENV32BIT
  #endif
#endif

#if __GNUC__
  #if __x86_64__ || __ppc64__
    #define ENV64BIT
  #else
    #define ENV32BIT
  #endif
#endif


namespace mcl {
#define MAX_VLA_LENGTH 5000
#define MCL_DATA_TYPE_DOUBLE
typedef double Real; /**< Real type */
typedef std::complex<Real> Complex; /**< Complex type */
  
#ifdef ENV64BIT
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
  
  template< typename... argv >
  void LogError(const char* format, argv... args) {
    if (output_type_ == kNone) { return; }
    
    const size_t SIZE = std::snprintf( NULL, 0, format, args... );
    
    std::string output;
    output.resize(SIZE+1);
    std::snprintf( &(output[0]), SIZE+1, format, args... );
    
    if (output_type_ == kCerr) {
      std::cerr<<output<<std::endl;
    } else if (output_type_ == kOutputFile) {
      // TODO: implement me.
    }
  }
  
  void SetOutputType(OutputType output_type) {
    output_type_ = output_type;
  }
  
  
private:
  Logger() : output_type_(kCerr) {}
  ~Logger() {}
  Logger(const Logger&);
  const Logger& operator= (const Logger&);
  OutputType output_type_;
};
  

} // namespace mcl



#endif
