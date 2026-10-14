# Changelog

Notable changes to this project are documented here.

## [Unreleased]

### Added

- Added the MIT license.
- Added non-blocking ADS1115 sampling with sample freshness and conversion-timeout checks.
- Added a battery-capacity menu that integrates mAh and Wh and stops on the configured voltage cutoff, runtime limit, or sensor fault.
- Added 20 Hz voltage/current CSV telemetry for Serial Oscilloscope, with voltage on channel 1 and current on channel 2.
- Added serial setup instructions for the oscilloscope connection.

### Changed

- Updated the LCD display cache and fixed-buffer numeric formatting to reduce repeated display writes and dynamic string allocation.
- Placed Battery Capacity directly after Constant Resistance and aligned its screen with the existing menu selection style. Fixed-width incremental rows avoid clearing the LCD every loop.
- Set the PID interval to 25 ms (40 Hz nominal) and reduced the default Ki from 400 to 40 for slower integral action; Kp and Kd remain zero.
- Corrected ESP32 ADC resolution configuration to 12 bits and added thermistor input validation.
- Removed stray library debug output so the serial telemetry stream contains only numeric CSV records.

### Verification

- Clean compilation and upload of the final firmware to the ESP32-D0WD-V3 on COM3 succeeded. The build reported the bundled LCD library's AVR architecture notice.
- Host simulation covered capacity integration, voltage-cutoff and runtime completion, overtemperature and sensor faults, and timer wraparound.
- A USB-only COM3 capture after upload produced 52 numeric voltage/current CSV rows at 115200 baud. With no input source or load connected, measurement calibration and control stability under load remain **NOT VERIFIED**.
