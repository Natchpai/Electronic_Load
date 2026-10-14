# Changelog

Notable firmware changes are documented here.

## [Unreleased]

## [2.0] - 2026-10-14

### Added

- Added a Battery Capacity test that integrates mAh and Wh and stops at its voltage cutoff, runtime limit, or a sensor or temperature fault.
- Added 20 Hz CSV voltage and current telemetry for Serial Oscilloscope.

### Changed

- Set the PID sample interval to 25 ms (40 Hz nominal), with default gains Kp = 0.2, Ki = 10, and Kd = 0.
- Limited editable PID gains to 0–0.8 for Kp/Kd and 0–15 for Ki.
- Set automatic fan thresholds to 35 °C on and 30 °C off.
- Require fresh valid sensor readings, input voltage of at least 0.1 V, and a positive current setpoint before enabling the load.
- Use cached, incremental LCD updates and keep the active Battery Capacity test setpoint through menu updates.

### Fixed

- Prevented Battery Capacity mode from clearing the LCD and resetting the load-off state on each loop.
- Distinguished temperature-sensor faults from ADC faults and displayed the thermistor's raw ADC reading on its fault screen.
- Retained the last valid temperature through brief ADC glitches and failed safe after the temperature reading became stale.

### Verification

- Host-side C++ checks passed for capacity integration, temperature-sensor fault handling, and over-temperature handling.
- Full ESP32 compilation and upload for this release are **NOT VERIFIED**. Loaded measurement calibration and control performance are also **NOT VERIFIED**.
