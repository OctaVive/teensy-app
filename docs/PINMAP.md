# Teensy 4.1 Pin Map — sACN/DMX LED Node

## WS2812B LED Strips (10 outputs)

| Strip | GPIO | Notes |
|-------|------|-------|
| 0 | 2 | 74HCT245 level shifter |
| 1 | 3 | |
| 2 | 4 | |
| 3 | 5 | |
| 4 | 6 | |
| 5 | 9 | |
| 6 | 10 | |
| 7 | 11 | |
| 8 | 12 | |
| 9 | 16 | |

All strips require equal LED count (default 300) for parallel DMA output.

## DMX512 Ports (8 half-duplex RS-485)

| Port | UART | TX | RX |
|------|------|----|----|
| 0 | Serial1 | 1 | 0 |
| 1 | Serial2 | 8 | 7 |
| 2 | Serial3 | 14 | 15 |
| 3 | Serial4 | 17 | 18 |
| 4 | Serial5 | 20 | 22 |
| 5 | Serial6 | 24 | 25 |
| 6 | Serial7 | 28 | 29 |
| 7 | Serial8 | 35 | 34 |

Use ADM2587E or MAX13487E (auto-direction) transceivers on carrier PCB.

## Other

| Function | Resource |
|----------|----------|
| Ethernet | Built-in RMII + PJRC magjack kit |
| SD Card | Onboard SDIO slot (config.json) |
| Status LED | 13 (built-in) |

## Reserved — do not use

- Pins 0, 1, 7, 8, 14, 15, 17, 18, 20, 22, 24, 25, 28, 29, 34, 35 (DMX UART)
- Pins 36–42 if USB host needed
- SDIO pins if SD logging extended
