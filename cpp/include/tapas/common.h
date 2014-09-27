#ifndef TAPAS_COMMON_H_
#define TAPAS_COMMON_H_

#include <string>
#include <cstdlib>
#include <sstream>
#include <cassert>
#include <iostream>

#define TAPAS_DEBUG

namespace tapas {

using std::string;
using std::ostream;

typedef int index_t;

inline void Exit(int status, const char *file, const char *func, int line) {
  if (status) {
    std::cerr << "Exiting at " << file << "@" << func << "#" << line << std::endl;
  }
  std::exit(status);  
}

#ifdef TAPAS_DEBUG
#define TAPAS_ASSERT(c) assert(c)
#else
#define TAPAS_ASSERT(c) do {} while (0)
#endif

#define TAPAS_DIE() do {                        \
    Exit(1, __FILE__, __FUNCTION__, __LINE__);   \
  } while (0)

// Special class to indicate none 
class NONE {};

class StringJoin {
  const std::string sep_;
  bool first_;
  std::ostringstream ss_;
 public:
  StringJoin(const string &sep=", "): sep_(sep), first_(true) {}
  void append(const string &s) {
    if (!first_) ss_ << sep_;
    ss_ << s;
    first_ = false;
  }
        
  template <class T>
  std::ostringstream &operator<< (const T &s) {
    if (!first_) ss_ << sep_;
    ss_ << s;
    first_ = false;
    return ss_;
  }
  std::string get() const {
    return ss_.str();
  }
  std::string str() const {
    return get();
  }
};

inline std::ostream& operator<<(std::ostream &os,
                                const StringJoin &sj) {
  return os << sj.get();
}

}

#endif /* TAPAS_COMMON_ */
