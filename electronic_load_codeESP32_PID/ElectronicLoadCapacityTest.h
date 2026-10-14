#pragma once

#include <stdint.h>

class ElectronicLoadCapacityTest {
public:
  enum State : uint8_t { IDLE, RUNNING, COMPLETE, STOPPED, FAULT };
  enum Reason : uint8_t {
    NONE,
    NOT_CONFIGURED,
    INPUT_NOT_READY,
    VOLTAGE_CUTOFF,
    RUNTIME_LIMIT,
    OVER_TEMPERATURE,
    SENSOR_FAILURE,
    USER_STOP
  };

  ElectronicLoadCapacityTest();
  bool start(uint32_t now, double currentLimit, double cutoffVoltage,
             double maximumCurrent, double voltage, double current,
             double temperature, bool sensorsValid, bool temperatureValid,
             double maximumTemperature);
  void update(uint32_t now, bool freshCurrent, bool freshVoltage,
              double voltage, double current, double temperature,
              bool sensorsValid, bool temperatureValid,
              double maximumTemperature, uint32_t maximumRuntimeMs);
  void stop(uint32_t now);
  void reset(uint32_t now);

  State state() const;
  Reason reason() const;
  uint32_t elapsedMs(uint32_t now) const;
  double milliampHours() const;
  double wattHours() const;
  bool running() const;

private:
  State state_;
  Reason reason_;
  uint32_t startedAt_;
  uint32_t lastIntegrationAt_;
  uint32_t stoppedAt_;
  double cutoffVoltage_;
  double milliampHours_;
  double wattHours_;
  double previousCurrent_;
  double previousPower_;
  bool previousSampleValid_;
};
