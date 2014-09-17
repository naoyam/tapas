#include <iostream>
#include "tapas/logging.h"

using namespace std;

using namespace tapas;
using namespace tapas::logging;

int main(int argc, char *argv[]) {
  TAPAS_LOG_ERROR() << "error test" << endl;
  TAPAS_LOG_WARNING() << "warning test" << endl;
  TAPAS_LOG_INFO() << "info test" << endl;  
  TAPAS_LOG_DEBUG() << "debug test" << endl;

  Logger debug_logger(std::cerr, TAPAS_LOG_LEVEL_INFO);
  TAPAS_LOG_DEBUG_STREAM(debug_logger) << "custom logger" << endl;

}


