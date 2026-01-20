#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <string>

#include "platform.h"

// D2XX header (from FTDI official package)
#include "ftd2xx.h"

static void print_usage(const char* exe) {
  std::cout
    << "Usage:\n"
    << "  " << exe << " <bits6> [duration_ms] [index] [--inv]\n\n"
    << "Arguments:\n"
    << "  <bits6>        6-bit binary string, e.g. 010101\n"
    << "                Leftmost is D5, rightmost is D0.\n"
    << "  [duration_ms]  Hold time in milliseconds (integer). Default: 17 (about 1/60 sec)\n"
    << "  [index]        FTDI device index for FT_Open(). Default: 0\n\n"
    << "Options:\n"
    << "  --inv          Invert output (active-low helper). Output becomes (~bits) & 0x3F\n"
    << "  -h, --help     Show this help\n\n"
    << "Examples:\n"
    << "  " << exe << " 010101\n"
    << "  " << exe << " 010101 60\n"
    << "  " << exe << " 010101 60 1\n"
    << "  " << exe << " 010101 60 1 --inv\n";
}

static bool parse_bits6_to_mask(const std::string& bits, uint8_t& out_mask) {
  if (bits.size() != 6) return false;
  uint8_t v = 0;
  for (char c : bits) {
    if (c != '0' && c != '1') return false;
    v = static_cast<uint8_t>((v << 1) | (c == '1' ? 1 : 0));
  }
  out_mask = v;
  return true;
}

static bool parse_u32(const char* s, uint32_t& out) {
  if (!s || !*s) return false;
  char* endp = nullptr;
  unsigned long v = std::strtoul(s, &endp, 10);
  if (endp == s || *endp != '\0') return false;
  if (v > 0xFFFFFFFFul) return false;
  out = static_cast<uint32_t>(v);
  return true;
}

static bool ft_ok(FT_STATUS st, const char* what) {
  if (st == FT_OK) return true;
  std::cerr << what << " failed: " << static_cast<int>(st) << "\n";
  return false;
}

int main(int argc, char** argv) {
  if (argc < 2) {
    print_usage(argv[0]);
    return 1;
  }

  bool invert = false;
  std::string bits;
  uint32_t duration_ms = 17; // default ~1/60 sec
  uint32_t index_u32 = 0;

  int positional = 0;
  for (int i = 1; i < argc; ++i) {
    std::string a = argv[i];

    if (a == "--inv") {
      invert = true;
      continue;
    }
    if (a == "-h" || a == "--help") {
      print_usage(argv[0]);
      return 0;
    }
    if (!a.empty() && a[0] == '-') {
      std::cerr << "Unknown option: " << a << "\n";
      print_usage(argv[0]);
      return 1;
    }

    ++positional;
    if (positional == 1) {
      bits = a;
    } else if (positional == 2) {
      if (!parse_u32(argv[i], duration_ms) || duration_ms == 0) {
        std::cerr << "Invalid duration_ms: " << a << "\n";
        print_usage(argv[0]);
        return 1;
      }
    } else if (positional == 3) {
      if (!parse_u32(argv[i], index_u32)) {
        std::cerr << "Invalid index: " << a << "\n";
        print_usage(argv[0]);
        return 1;
      }
    } else {
      std::cerr << "Too many positional arguments.\n";
      print_usage(argv[0]);
      return 1;
    }
  }

  if (bits.empty()) {
    print_usage(argv[0]);
    return 1;
  }

  uint8_t mask = 0;
  if (!parse_bits6_to_mask(bits, mask)) {
    std::cerr << "Invalid bits (need 6 chars of 0/1): " << bits << "\n";
    print_usage(argv[0]);
    return 1;
  }

  // D0..D5 outputs (6-bit)
  const UCHAR dir_mask = 0x3F;
  UCHAR out = static_cast<UCHAR>(mask & 0x3F);
  if (invert) {
    out = static_cast<UCHAR>((~out) & 0x3F);
  }

  FT_HANDLE h = nullptr;

  FT_STATUS st = FT_Open(static_cast<int>(index_u32), &h);
  if (!ft_ok(st, "FT_Open")) return 2;

  ft_ok(FT_ResetDevice(h), "FT_ResetDevice");
  ft_ok(FT_Purge(h, FT_PURGE_RX | FT_PURGE_TX), "FT_Purge");
  ft_ok(FT_SetBaudRate(h, 115200), "FT_SetBaudRate");
  ft_ok(FT_SetLatencyTimer(h, 2), "FT_SetLatencyTimer");

  // 0x01 = asynchronous bit-bang
  st = FT_SetBitMode(h, dir_mask, 0x01);
  if (!ft_ok(st, "FT_SetBitMode(ASYNC_BITBANG)")) {
    FT_Close(h);
    return 3;
  }

  DWORD written = 0;
  st = FT_Write(h, &out, 1, &written);
  if (st != FT_OK || written != 1) {
    std::cerr << "FT_Write(out) failed: st=" << static_cast<int>(st)
              << " written=" << static_cast<unsigned long>(written) << "\n";
    FT_SetBitMode(h, 0x00, 0x00);
    FT_Close(h);
    return 4;
  }

  platform::sleep_ms(duration_ms);

  // Clear
  out = 0x00;
  written = 0;
  st = FT_Write(h, &out, 1, &written);
  if (st != FT_OK || written != 1) {
    std::cerr << "FT_Write(clear) failed: st=" << static_cast<int>(st)
              << " written=" << static_cast<unsigned long>(written) << "\n";
    // Continue cleanup.
  }

  FT_SetBitMode(h, 0x00, 0x00);
  FT_Close(h);

  return 0;
}
