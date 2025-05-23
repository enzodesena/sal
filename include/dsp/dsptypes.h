/*
 Spatial Audio Library (SAL)
 Copyright (c) 2012-24, Enzo De Sena
 All rights reserved.

 Authors: Enzo De Sena, enzodesena@gmail.com
 */

#include <assert.h>

#ifndef SAL_DSP_MCLTYPES_H
#define SAL_DSP_MCLTYPES_H

#ifndef PI
#define PI 3.141592653589793238462643383279502884197169399375105820974944
#endif

#define ASSERT(arg) assert(arg)
#define ASSERT_WITH_MESSAGE(condition, message) ASSERT(condition&& message)
#define AVOID_UNUSED_WARNING(arg) (void)(arg);

#include <complex>
#include <fstream>
#include <iostream>
#include <vector>

#if _WIN32 || _WIN64
#if _WIN64
#define SAL_DSP_ENV64BIT 1
#else
#define SAL_DSP_ENV32BIT 1
#endif
#elif __GNUC__
#if __x86_64__ || __ppc64__
#define SAL_DSP_ENV64BIT 1
#else
#define SAL_DSP_ENV32BIT 1
#endif
#endif

#if _WIN32 || _WIN64
#define SAL_DSP_ENVWINDOWS 1
#elif __APPLE__
#define SAL_DSP_ENVAPPLE 1
#elif __arm__ || __aarch64__  // Since this elif comes second, priority is given
                              // to APPLE descriptor
#define SAL_DSP_ENVARM 1
#else
#define SAL_DSP_ENVOTHER 1
#endif

#ifdef SAL_DSP_ENVAPPLE
#define SAL_DSP_APPLE_ACCELERATE 1
#elif SAL_DSP_ENVARM
#define SAL_DSP_NEON_ACCELERATE 1
#elif SAL_DSP_ENVWINDOWS
#define SAL_DSP_AVX_ACCELERATE 1
#else  // SAL_DSP_ENVOTHER
#define SAL_DSP_NO_ACCELERATE 1
#endif

// Exclude multiply and multiply-add as the compiler is able to do a better job
#define SAL_DSP_APPLE_ACCELERATE_MMA 0

#define SAL_DSP_MAX_VLA_LENGTH 16384

#if SAL_DSP_ENVWINDOWS
#define SAL_DSP_STACK_ALLOCATE(type, variable, size) \
  type* variable = (type*)alloca((size) * sizeof(type));
#else
#define SAL_DSP_STACK_ALLOCATE(type, variable, size) type variable[(size)];
#endif

#if SAL_DSP_ENVWINDOWS
#include <intrin.h>
#endif

namespace sal {

namespace dsp {

#define SAL_DSP_DATA_TYPE_DOUBLE 1

#if SAL_DSP_DATA_TYPE_DOUBLE
typedef double Real; /**< Real type */
#else
typedef float Real; /**< Real type */
#endif

typedef std::complex<Real> Complex; /**< Complex type */

#ifdef SAL_DSP_ENV64BIT
typedef unsigned long long UInt; /**< Unsigned int type */
typedef long long Int;           /**< Int type */
#else                            // If it is 32 bits or unknown then...
typedef unsigned long UInt; /**< Unisgned int type */
typedef long Int;           /**< Int type */
#endif

typedef std::complex<Real> Complex;

#if SAL_DSP_DATA_TYPE_DOUBLE
static const Complex imaginary_unit(0.0, 1.0);
#else
static const Complex imaginary_unit(0.0f, 1.0f);
#endif

/** Singleton class carrying information about the runtime environment */
class RuntimeArchInfo {
 public:
  static RuntimeArchInfo& GetInstance() {
    static RuntimeArchInfo instance;
    return instance;
  }

  bool IsAvxSupported() {
#if defined(SAL_DSP_ENVWINDOWS)
    if (!system_has_been_polled_) {
      int cpu_info[4];
      __cpuid(cpu_info, 0);
      if (cpu_info[0] >= 1) {
        __cpuidex(cpu_info, 1, 0);
        if ((cpu_info[2] & (1 << 28)) != 0) {
          avx_supported_ = true;
        }
      }
      system_has_been_polled_ = true;
    }
#endif

    return avx_supported_;
  }

 private:
  bool avx_supported_ = false;
  bool system_has_been_polled_ = false;
};

/** Singleton logger class */
class Logger {
 public:
  static Logger& GetInstance() {
    static Logger instance;
    return instance;
  }

  enum OutputType { kNone, kCerr, kOutputFile };

  void LogErrorToCerr(std::string output) { std::cerr << output << std::endl; }

  void LogErrorToCerr(const char* output) { std::cerr << output << std::endl; }

  void LogError(const char* format) {
    if (output_type_ == kNone) {
      return;
    }

    const size_t SIZE = snprintf(NULL, 0, "%s", format);

    std::string output;
    output.resize(SIZE + 1);
    snprintf(&(output[0]), SIZE + 1, "%s", format);

    if (output_type_ == kCerr) {
      std::cerr << output << std::endl;
    } else if (output_type_ == kOutputFile) {
      log_string_.append("\n");
      log_string_.append(format);
    }
  }

  template <typename... argv>
  void LogError(const char* format, argv... args) {
    if (output_type_ == kNone) {
      return;
    }

    const size_t SIZE = snprintf(NULL, 0, format, args...);

    std::string output;
    output.resize(SIZE + 1);
    snprintf(&(output[0]), SIZE + 1, format, args...);

    if (output_type_ == kCerr) {
      std::cerr << output << std::endl;
    } else if (output_type_ == kOutputFile) {
      log_string_.append("\n");
      log_string_.append(format);
    }
  }

  void SetOutputType(OutputType output_type) { output_type_ = output_type; }

  void SetOutputFile(const std::string& log_output_file) {
    log_output_file_ = log_output_file;
  }

 private:
  Logger() : output_type_(kCerr), log_output_file_("err.log") {}
  ~Logger() {
    if (log_string_.length() > 0) {
      std::cerr << "Writing logger out to " << log_output_file_ << std::endl;
      std::ofstream output_stream(log_output_file_);
      output_stream << log_string_;
      output_stream.close();
    }
  }

  Logger(const Logger&) = delete;
  const Logger& operator=(const Logger&) = delete;

  OutputType output_type_;

  std::string log_string_;
  std::string log_output_file_;
};

} // namespace dsp

} // namespace sal

#endif
