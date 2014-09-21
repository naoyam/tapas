#ifndef TAPAS_LOGGING_H_
#define TAPAS_LOGGING_H_

#include <iostream>
#include <string>

#define TAPAS_LOG_LEVEL_ERROR 0
#define TAPAS_LOG_LEVEL_WARNING 1
#define TAPAS_LOG_LEVEL_INFO 2
#define TAPAS_LOG_LEVEL_DEBUG 3
#ifndef TAPAS_LOG_LEVEL
#define TAPAS_LOG_LEVEL TAPAS_LOG_LEVEL_DEBUG
#endif

#define TAPAS_TRACE 1

namespace tapas  {
namespace logging {

using std::ostream;
using std::string;
using std::cerr;
using std::endl;

inline string file_path_basename(const char *fp) {
  string fps(fp);
  size_t pos = fps.rfind('/');
  if (pos == string::npos) {
    return fp;
  } else {
    return fps.substr(pos + 1);
  }
}
    
class Logger {
  ostream &os_;
  bool enabled_;
  int level_;
 public:	
  Logger(ostream &os=std::cerr, int level=TAPAS_LOG_LEVEL)
      : os_(os), enabled_(true), level_(level) {}

  void LogHeader(const string& error_level, const char *file,
                 const char *func, int line) const {
    if (enabled_) {
      os_ << "[" << error_level << ": ";
      os_ << func << "@"
          << file_path_basename(file) << "#" << line << "] ";
    }
  }

  Logger LogError(const char *file, const char *func, int line) {
    if (enabled_ && level_ >= TAPAS_LOG_LEVEL_ERROR) {
      LogHeader("ERROR", file, func, line, 2);
    } else {
      enabled_ = false;
    }
    return *this;    
  }

  Logger LogWarning(const char *file, const char *func, int line) {
    if (enabled_ && level_ >= TAPAS_LOG_LEVEL_WARNING) {
      LogHeader("WARNING", file, func, line, 0);
    } else {
      enabled_ = false;
    }
    return *this;
  }

  Logger LogInfo(const char *file, const char *func, int line) {
    if (enabled_ && level_ >= TAPAS_LOG_LEVEL_INFO) {
      LogHeader("INFO", file, func, line, 3);
    } else {
      enabled_ = false;
    }
    return *this;
  }

  Logger LogDebug(const char *file, const char *func, int line) {
    if (enabled_ && level_ >= TAPAS_LOG_LEVEL_DEBUG) {
      LogHeader("DEBUG", file, func, line, 2);
    } else {
      enabled_ = false;
    }
    return *this;
  }

  template <class T>
  Logger operator<<(const T &x) {
    if (enabled_) os_ << x;
    return *this;
  }

  Logger operator<<(ostream& (*pf)(ostream&)) {
    if (enabled_) pf(os_);
    return *this;
  }
}; // class Logger

#if 0
template <class T>
Logger &operator<<(Logger &logger, const T&x) {
  return logger<<(x);
}
#endif
} // namespace logging
} // namespace tapas

#define TAPAS_LOG_DEFAULT_STREAM (tapas::logging::Logger())

#if TAPAS_LOG_LEVEL >= TAPAS_LOG_LEVEL_ERROR
#define TAPAS_LOG_ERROR()                                       \
  (TAPAS_LOG_DEFAULT_STREAM.LogError(__FILE__, __FUNCTION__, __LINE__))
#else
#define TAPAS_LOG_ERROR() if (0) std::cerr 
#endif /* TAPAS_LOG_LEVEL >= TAPAS_LOG_LEVEL_ERROR */

#if TAPAS_LOG_LEVEL >= TAPAS_LOG_LEVEL_WARNING
#define TAPAS_LOG_WARNING()                                     \
  (TAPAS_LOG_DEFAULT_STREAM.LogWarning(__FILE__, __FUNCTION__, __LINE__))  
#else
#define TAPAS_LOG_WARNING() if (0) std::cerr 
#endif /* TAPAS_LOG_LEVEL >= TAPAS_LOG_LEVEL_WARNING */

#if TAPAS_LOG_LEVEL >= TAPAS_LOG_LEVEL_INFO
#define TAPAS_LOG_INFO()                                        \
  (TAPAS_LOG_DEFAULT_STREAM.LogInfo(__FILE__, __FUNCTION__, __LINE__))  
#else
#define TAPAS_LOG_INFO() if (0) std::cerr 
#endif /* TAPAS_LOG_LEVEL >= TAPAS_LOG_LEVEL_INFO */

#if TAPAS_LOG_LEVEL >= TAPAS_LOG_LEVEL_DEBUG
#define TAPAS_LOG_DEBUG()                                        \
  (TAPAS_LOG_DEFAULT_STREAM.LogDebug(__FILE__, __FUNCTION__, __LINE__))  
#else
#define TAPAS_LOG_DEBUG() if (0) std::cerr 
#endif /* TAPAS_LOG_LEVEL >= TAPAS_LOG_LEVEL_DEBUG */

#if TAPAS_LOG_LEVEL >= TAPAS_LOG_LEVEL_ERROR
#define TAPAS_LOG_ERROR()                                       \
  (TAPAS_LOG_DEFAULT_STREAM.LogError(__FILE__, __FUNCTION__, __LINE__))
#else
#define TAPAS_LOG_ERROR() if (0) std::cerr 
#endif /* TAPAS_LOG_LEVEL >= TAPAS_LOG_LEVEL_ERROR */

#if TAPAS_LOG_LEVEL >= TAPAS_LOG_LEVEL_WARNING
#define TAPAS_LOG_WARNING()                                     \
  (TAPAS_LOG_DEFAULT_STREAM.LogWarning(__FILE__, __FUNCTION__, __LINE__))  
#else
#define TAPAS_LOG_WARNING() if (0) std::cerr 
#endif /* TAPAS_LOG_LEVEL >= TAPAS_LOG_LEVEL_WARNING */

#if TAPAS_LOG_LEVEL >= TAPAS_LOG_LEVEL_INFO
#define TAPAS_LOG_INFO()                                        \
  (TAPAS_LOG_DEFAULT_STREAM.LogInfo(__FILE__, __FUNCTION__, __LINE__))  
#else
#define TAPAS_LOG_INFO() if (0) std::cerr 
#endif /* TAPAS_LOG_LEVEL >= TAPAS_LOG_LEVEL_INFO */

#define TAPAS_LOG_ERROR_STREAM(s)                       \
  ((s).LogError(__FILE__, __FUNCTION__, __LINE__))  
#define TAPAS_LOG_WARNING_STREAM(s)                     \
  ((s).LogWarning(__FILE__, __FUNCTION__, __LINE__))  
#define TAPAS_LOG_INFO_STREAM(s)                        \
  ((s).LogInfo(__FILE__, __FUNCTION__, __LINE__))  
#define TAPAS_LOG_DEBUG_STREAM(s)                       \
  ((s).LogDebug(__FILE__, __FUNCTION__, __LINE__))  


#endif /* TAPAS_LOGGING_H_ */
