# Project Log

## 2026-10-14 — Battery Capacity load enable bug

**Context:** Battery Capacity showed a running test, but measured and supply current stayed at zero; other modes worked.

**Decisions:**
- [ACCEPTED] Update `previous_mode` in the Battery menu handler and reset `onLoad` only when entering the menu.

**Findings:**
- The missing state update made the shared transition logic clear the LCD and force `onLoad` off on every loop. The fix compiled, uploaded to COM3, and the user confirmed Battery Capacity works.

**References:** `electronic_load_codeESP32_PID/ElectronicLoadUI.cpp` (`runMenu()`, Battery Capacity branch).

## 2026-10-14 — Current ADC scaling discrepancy

**Context:** Reviewed the shared current measurement path while diagnosing Battery Capacity's zero-current symptom.

**Findings:**
- **NOT VERIFIED:** The firmware multiplies ADS1115 counts by `0.001250`; at the configured ±4.096 V gain, the ADC scale is about `0.000125 V/count`. The schematic describes the current-sense output as approximately 1 V/A. This suggests the reported current may be scaled about 10× high.
- Left unchanged because other modes were reported working and no known-current calibration measurement was available. This did not explain the Battery-only load-disable bug.

**References:** `electronic_load_codeESP32_PID/electronic_load_codeESP32_PID.ino` (`serviceSensorSampler()`); `PICTURE/Electronic_Load_sch_color_page-0002.jpg`.
