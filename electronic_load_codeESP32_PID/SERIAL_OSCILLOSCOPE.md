# Serial Oscilloscope

The firmware sends one numeric CSV record every 50 ms (20 samples/s):

```text
<selected voltage in volts>,<measured current in amps>
```

Serial Oscilloscope assigns comma-separated values to channels in order, so CH1 is the selected voltage input and CH2 is current. Records end with CR/LF. Configure the application for the ESP32's COM port at **115200 baud**. Close Arduino Serial Monitor before opening that port in the oscilloscope because a serial port can only have one active client.

The stream contains measurements only; do not add labels or diagnostic messages to this serial port while plotting. With USB power and no test source connected, the plots can confirm record framing and channel operation, but cannot validate voltage/current calibration or load behavior.
