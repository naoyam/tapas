#ifndef TAPAS_COMMON_H_
#define TAPAS_COMMON_H_

#include <string>
#include <sstream>
#include <cassert>

#define TAPAS_DEBUG

namespace tapas {

using std::string;
using std::ostream;

typedef unsigned index_t;

// Special class to indicate none 
class NONE {};

class StringJoin {
  const std::string sep;
  bool first;
  std::ostringstream ss;
 public:
  StringJoin(const string &sep=", "): sep(sep), first(true) {}
  void append(const string &s) {
    if (!first) ss << sep;
    ss << s;
    first = false;
  }
        
  template <class T>
  std::ostringstream &operator<< (const T &s) {
    if (!first) ss << sep;
    ss << s;
    first = false;
    return ss;
  }
  std::string get() const {
    return ss.str();
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
