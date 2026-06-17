# Carrier PCB — Teensy 4.1 sACN/DMX LED Node

Production carrier board for the LED node firmware. This document defines the target hardware architecture for prototype and production builds.

## Block Diagram

```
                    ┌─────────────────────────────────────┐
  5V IN ───────────►│ Power input (terminal block)        │
                    │  • Teensy VIN (5V)                  │
                    │  • Isolated DC-DC rails for DMX     │
                    └─────────────────────────────────────┘
                                      │
┌─────────────────────────────────────┴─────────────────────────────────────┐
│                              Carrier PCB                                     │
│  ┌──────────────┐   ┌─────────────┐   ┌──────────────────────────────────┐  │
│  │ Teensy 4.1   │   │ PJRC Eth    │   │ 10× SN74AHCT125 / 74HCT245       │  │
│  │ (socket)     │──►│ Magjack kit │   │ WS2812 data outputs J1–J10       │  │
│  └──────┬───────┘   └─────────────┘   └──────────────────────────────────┘  │
│         │ Serial1–8                                                          │
│         ▼                                                                      │
│  ┌──────────────────────────────────────────────────────────────────────────┐│
│  │ 8× ADM2587E isolated RS-485 transceiver → XLR-5 DMX OUT/IN (P1–P8)        ││
│  │  • 120Ω termination jumper per port (default OFF)                        ││
│  │  • TVS diode array on A/B                                                ││
│  │  • TX/RX activity LEDs                                                   ││
│  └──────────────────────────────────────────────────────────────────────────┘│
│  SD card (Teensy onboard) │ Status LED (D13) │ USB                           │
└──────────────────────────────────────────────────────────────────────────────┘
```

## Connectors

| Ref | Type | Function |
|-----|------|----------|
| J1–J10 | 3-pin JST or screw terminal | WS2812 data (GND, DATA, optional 5V reference) |
| P1–P8 | XLR-5 female | DMX512-A (pin 1 GND, 2 D−, 3 D+, 4/5 optional) |
| ETH1 | RJ45 magjack | 10/100 Ethernet (sACN) |
| PWR1 | 2-pin terminal | 5V for Teensy logic (not LED power) |

## WS2812 Outputs

- One **74HCT245** or **SN74AHCT125** channel per strip
- 100–220 Ω series resistor on each data line
- Common GND between Teensy and LED PSU
- LED power external (do not route strip power through Teensy)

## DMX Ports

Recommended transceiver: **ADM2587E** (integrated isolation + DC-DC).

Prototype alternative: **MAX13487E** (auto-direction, no DE/RE GPIO).

| Port | UART | Teensy TX | Teensy RX |
|------|------|-----------|-----------|
| P1 | Serial1 | 1 | 0 |
| P2 | Serial2 | 8 | 7 |
| P3 | Serial3 | 14 | 15 |
| P4 | Serial4 | 17 | 18 |
| P5 | Serial5 | 20 | 22 |
| P6 | Serial6 | 24 | 25 |
| P7 | Serial7 | 28 | 29 |
| P8 | Serial8 | 35 | 34 |

## BOM Summary (production)

| Qty | Part | Description |
|-----|------|-------------|
| 1 | Teensy 4.1 | Main controller |
| 1 | PJRC Ethernet kit | Magjack + passives |
| 10 | SN74AHCT125 or 74HCT245 | WS2812 level shift |
| 8 | ADM2587E | Isolated RS-485 |
| 8 | XLR-5 panel mount | DMX connectors |
| 10 | 3-pin output connector | LED strip data |
| 8 | 120Ω 0805 + jumper | DMX termination |
| 8 | SMAJ6.5CA or equivalent | DMX TVS |

## Bring-up Checklist

1. Power Teensy only; verify USB serial @ 115200
2. Insert SD with `config.json`; confirm boot log
3. Connect Ethernet; verify link + IP
4. One WS2812 strip on J1; run `teensy41_test` env first
5. One DMX port loopback or fixture test per port
6. Scale to full 10 strips + 8 DMX

## EMC Notes

- Keep WS2812 data traces short; ground plane under high-speed lines
- Isolate DMX earth from logic ground via ADM2587E
- Star-ground LED PSU returns; do not daisy-chain high-current GND through Teensy

## Revision Target

| Rev | Goal |
|-----|------|
| A | Prototype — 2 LED + 2 DMX validated |
| B | Full 10 LED + 8 DMX |
| C | Production — EN55032 pre-compliance |
