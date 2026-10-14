# ESP32 Electronic Load

Firmware and KiCad design files for a programmable DC electronic load controlled by an ESP32. The firmware measures voltage and current through an external ADC, drives the load through PWM, and provides a local LCD and rotary-encoder interface.

## Hardware

<p align="center">
  <img src="PICTURE/IMG_8593.JPG" alt="Electronic load hardware" width="600">
</p>


## Features

- Constant-current, constant-power, and constant-resistance modes
- Digital voltmeter and PID tuning menus
- Non-blocking ADC sampling and cached LCD updates
- Battery-capacity test mode with voltage cutoff, runtime limit, and mAh/Wh totals
- Voltage/current CSV telemetry for [Serial Oscilloscope](https://github.com/xioTechnologies/Serial-Oscilloscope)

## Repository Layout

| Path | Contents |
|---|---|
| `Electronic_load_esp32/` | Editable KiCad project, schematic, and PCB |
| `electronic_load_codeESP32_PID/` | ESP32 firmware sketch, display/UI/sensor modules, and bundled libraries |
| `CHANGELOG.md` | Confirmed project changes |
| `AGENTS.md` | Contributor guidance |

## Schematics

<p align="center">
  <img src="PICTURE/Electronic_Load_sch_color_page-0001.jpg" alt="Electronic load schematic, page 1" width="600">
</p>

<p align="center">
  <img src="PICTURE/Electronic_Load_sch_color_page-0002.jpg" alt="Electronic load schematic, page 2" width="600">
</p>

## Build Firmware

Install Arduino CLI and the ESP32 Arduino platform, then run this command from the repository root:

```powershell
arduino-cli compile --fqbn esp32:esp32:esp32 --libraries .\electronic_load_codeESP32_PID\Inc .\electronic_load_codeESP32_PID
```

The target used for the current build is `esp32:esp32:esp32`. Project libraries are kept under `electronic_load_codeESP32_PID/Inc`.


## Serial Oscilloscope

The firmware emits one numeric CSV row every 50 ms at 115200 baud. Channel 1 is the selected voltage input in volts; channel 2 is current in amps. Select the ESP32's COM port in Serial Oscilloscope and close Arduino Serial Monitor before connecting. See [serial setup details](electronic_load_codeESP32_PID/SERIAL_OSCILLOSCOPE.md).

## Verification and Hardware Notes

The committed firmware clean-compiled and was uploaded to an ESP32-D0WD-V3 on COM3; a USB-only capture after upload received 52 numeric CSV rows at 115200 baud. Default gains are Kp = 0, Ki = 40, and Kd = 0, so the default controller is integral-only. No external input source or load was connected, so ADC calibration, rated operating limits, thermal performance, and controller stability under load are **not verified**. Confirm sensor calibration and power-stage component ratings and cooling before applying power.

## License

Licensed under the [MIT License](LICENSE).