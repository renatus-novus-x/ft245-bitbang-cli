#ifdef _WIN32

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "platform.h"

namespace platform {

void sleep_ms(uint32_t ms) {
  ::Sleep(static_cast<DWORD>(ms));
}

} // namespace platform

#endif
