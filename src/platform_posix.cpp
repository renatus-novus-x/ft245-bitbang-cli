#ifndef _WIN32

#include <time.h>

#include "platform.h"

namespace platform {

void sleep_ms(uint32_t ms) {
  timespec ts;
  ts.tv_sec = static_cast<time_t>(ms / 1000u);
  ts.tv_nsec = static_cast<long>((ms % 1000u) * 1000000ul);
  while (nanosleep(&ts, &ts) == -1) {
    // Retry if interrupted.
  }
}

} // namespace platform

#endif
