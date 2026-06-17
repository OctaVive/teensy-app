# Integration And Soak Test Plan

This checklist verifies phase 6 integration goals from the technical plan.

## Prerequisites

- Teensy 4.1 flashed with `teensy41` firmware build
- SD card with `config.json`
- Ethernet connected to sACN source network
- At least one WS2812 strip connected
- DMX fixtures or loopback adapters for ports 0-7

## Integration Validation

1. Boot node and confirm startup logs:
   - mode
   - LED totals
   - sACN universe range
2. Verify pixel mode:
   - Send universes 1-18
   - Confirm linear mapping across strip boundaries
3. Verify effect mode:
   - Set `"mode": "effect"` in `config.json`, reboot
   - Send universe 100 channels 1-60
   - Confirm program select (`channel 4`) works per strip
4. Verify VirtualAll:
   - Use strip 0 control channels in effect mode
   - Confirm effects run continuously across all 10 strips
5. Verify DMX ports:
   - Set mixed input/output directions in config
   - Confirm all 8 ports initialize and process frames
6. Verify timeout behavior:
   - Stop sACN source for >1s
   - Confirm last-frame hold behavior remains stable

## 24-Hour Soak Procedure

1. Start continuous sACN stream at 40 fps:
   - Pixel mode for 12 hours
   - Effect mode for 12 hours
2. Keep DMX ports active throughout soak:
   - 4 ports in input mode
   - 4 ports in output mode
3. Record every 5 minutes:
   - Reported FPS
   - sACN packet counters
   - Link up/down events
   - Any watchdog reset
4. Pass criteria:
   - No firmware crash or lockup
   - No unexpected reboot
   - Output remains active and responsive
   - DMX processing remains active on all configured ports

## Post-Soak Artifacts

- Serial log capture (`serial-soak.log`)
- Final config file used for soak
- Summary report:
  - total runtime
  - packet counters
  - observed anomalies
