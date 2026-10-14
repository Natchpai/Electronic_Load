#pragma once

#define temp_sense_pin 34
#define buzzer_pin 32
#define red_button_pin 26
#define yellow_button_pin 25
#define rotary_button_pin 27
#define Encoder_DT 13
#define Encoder_CLK 14
#define PWM_pinOut 4
#define fanCtrl_pinOut 15
#define gateCtrl_pinOut 2

#define updateTemp_time 1000
#define updateSensor_time 500
#define updateVoltage_time 250
#define VIN_MIN 0.1

#define ADS_DATA_RATE 4
#define ADS_CONVERSION_WAIT_MS 8
#define ADS_CONVERSION_TIMEOUT_MS 100
#define PID_SAMPLE_INTERVAL_MS 25
#define SENSOR_STALE_TIMEOUT_MS 150
#define TEMPERATURE_STALE_TIMEOUT_MS 3000
#define SERIAL_TELEMETRY_INTERVAL_MS 50
#define CAPACITY_TELEMETRY_INTERVAL_MS 1000
#define ENABLE_PERF_DIAGNOSTICS 0
