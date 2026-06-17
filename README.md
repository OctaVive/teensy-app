# Teensy 4.1 sACN/DMX LED Node

Firmware for a Teensy 4.1 that drives 10 WS2812B strips (3,000 LEDs) via sACN (E1.31), with 8 independent DMX512 ports and a FastLED effect engine.

## Features

- **Pixel mode** — 18 sACN universes, sequential RGB mapping (170 pixels/universe)
- **Effect mode** — 11 FastLED effects, per-strip or virtual-all targeting via sACN universe 100
- **8 DMX ports** — configurable input/output (half-duplex, one UART per port)
- **Configuration** — `config.json` on SD card with embedded defaults fallback
- **40 fps** scheduler with watchdog and diagnostics

## Build

Requires [PlatformIO](https://platformio.org/).

```bash
python -m platformio run -e teensy41
python -m platformio run -e teensy41_test   # 30 LEDs/strip for bench testing
python -m platformio run -t upload -e teensy41
python -m platformio device monitor
```

## Configuration

Copy [`data/config.json`](data/config.json) to the root of a FAT32 SD card. Set `"mode"` to `"pixel"` or `"effect"`. Reboot after changes.

See [`docs/PINMAP.md`](docs/PINMAP.md) for GPIO assignments.

## sACN Mapping

### Pixel mode (default)

| Universes | Pixels |
|-----------|--------|
| 1–18 | 0–2999 (sequential RGB) |

### Effect mode

| Universe | Channels | Purpose |
|----------|----------|---------|
| 100 | 1–60 | 6 channels × 10 strips (R,G,B,Program,Dimmer,Speed) |
| 101 | 1–6 | Optional VirtualAll override |

## Hardware

See [`hardware/README.md`](hardware/README.md) for carrier PCB architecture.

Integration and 24-hour stability validation are documented in
[`docs/INTEGRATION_SOAK_TEST.md`](docs/INTEGRATION_SOAK_TEST.md).

## Libraries

- FastLED 3.9.8+
- QNEthernet
- TeensyDMX
- ArduinoJson 7
- SdFat
