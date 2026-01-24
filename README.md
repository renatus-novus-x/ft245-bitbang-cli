[![windows](https://github.com/renatus-novus-x/ft245-bitbang-cli/workflows/windows/badge.svg)](https://github.com/renatus-novus-x/ft245-bitbang-cli/actions?query=workflow%3Awindows)
[![macos](https://github.com/renatus-novus-x/ft245-bitbang-cli/workflows/macos/badge.svg)](https://github.com/renatus-novus-x/ft245-bitbang-cli/actions?query=workflow%3Amacos)
[![ubuntu](https://github.com/renatus-novus-x/ft245-bitbang-cli/workflows/ubuntu/badge.svg)](https://github.com/renatus-novus-x/ft245-bitbang-cli/actions?query=workflow%3Aubuntu)

# ft245-bitbang-cli
Cross-platform CLI to pulse 6-bit FT245 async bit-bang outputs via FTDI D2XX (binary pattern + duration, optional device index, active-low invert).

## Usage

```
ft245-bitbang-cli <bits> [duration_ms] [device_index] [--inv]
```

- `<bits>`: 6-bit binary string (e.g. `010101`).  
  Leftmost is `D5`, rightmost is `D0`. Only `D0..D5` are output.
- `[duration_ms]`: Hold time in milliseconds (integer). Default: `17` (about 1/60 sec).
- `[device_index]`: FTDI device index passed to `FT_Open(index, ...)`. Default: `0`.
- `--inv`: Invert outputs (useful for active-low wiring). Output becomes `(~bits) & 0x3F`.

### Examples
Pulse `010101` for ~1/60 sec (default):

```
ft245-bitbang-cli 010101
```

Pulse `010101` for 100 ms(1/10 sec):
```
ft245-bitbang-cli 010101 100
```

Use device index 1:
```
ft245-bitbang-cli 010101 100 1
```

Active-low (invert outputs):
```
ft245-bitbang-cli 010101 100 1 --inv
```

## Download
- [ft245-bitbang-cli.exe (windows)](https://raw.githubusercontent.com/renatus-novus-x/ft245-bitbang-cli/main/bin/ft245-bitbang-cli.exe)
- [ft245-bitbang-cli-macos.zip (macos)](https://raw.githubusercontent.com/renatus-novus-x/ft245-bitbang-cli/main/bin/ft245-bitbang-cli-macos.zip)
- [ft245-bitbang-cli (ubuntu)](https://raw.githubusercontent.com/renatus-novus-x/ft245-bitbang-cli/main/bin/ft245-bitbang-cli)
