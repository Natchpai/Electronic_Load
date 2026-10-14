#include "ElectronicLoadCapacityTest.h"

#include <math.h>

ElectronicLoadCapacityTest::ElectronicLoadCapacityTest()
  : state_(IDLE), reason_(NONE), startedAt_(0), lastIntegrationAt_(0),
    stoppedAt_(0), cutoffVoltage_(0.0), milliampHours_(0.0), wattHours_(0.0),
    previousCurrent_(0.0), previousPower_(0.0), previousSampleValid_(false) {}

bool ElectronicLoadCapacityTest::start(uint32_t now, double currentLimit,
                                       double cutoffVoltage, double maximumCurrent,
                                       double voltage, double current,
                                       double temperature, bool sensorsValid,
                                       bool temperatureValid,
                                       double maximumTemperature) {
  if (state_ == RUNNING) return false;
  if (cutoffVoltage <= 0.1 || currentLimit <= 0.0 ||
      currentLimit > maximumCurrent) {
    reason_ = NOT_CONFIGURED;
    return false;
  }
  if (!sensorsValid || !temperatureValid || !isfinite(voltage) ||
      !isfinite(current) || !isfinite(temperature) ||
      voltage < cutoffVoltage || temperature >= maximumTemperature) {
    reason_ = INPUT_NOT_READY;
    return false;
  }

  state_ = RUNNING;
  reason_ = NONE;
  cutoffVoltage_ = cutoffVoltage;
  startedAt_ = now;
  lastIntegrationAt_ = now;
  stoppedAt_ = now;
  milliampHours_ = 0.0;
  wattHours_ = 0.0;
  previousCurrent_ = current;
  previousPower_ = voltage * current;
  previousSampleValid_ = true;
  return true;
}

void ElectronicLoadCapacityTest::update(uint32_t now, bool freshCurrent,
                                        bool freshVoltage, double voltage,
                                        double current, double temperature,
                                        bool sensorsValid, bool temperatureValid,
                                        double maximumTemperature,
                                        uint32_t maximumRuntimeMs) {
  if (state_ != RUNNING) return;

  if (!sensorsValid || !isfinite(voltage) || !isfinite(current)) {
    state_ = FAULT;
    reason_ = SENSOR_FAILURE;
    stoppedAt_ = now;
    return;
  }
  if (!temperatureValid || !isfinite(temperature)) {
    state_ = FAULT;
    reason_ = TEMPERATURE_SENSOR_FAILURE;
    stoppedAt_ = now;
    return;
  }
  if (temperature >= maximumTemperature) {
    state_ = FAULT;
    reason_ = OVER_TEMPERATURE;
    stoppedAt_ = now;
    return;
  }
  if (freshVoltage && voltage <= cutoffVoltage_) {
    state_ = COMPLETE;
    reason_ = VOLTAGE_CUTOFF;
    stoppedAt_ = now;
    return;
  }

  if (maximumRuntimeMs > 0 && now - startedAt_ >= maximumRuntimeMs) {
    state_ = COMPLETE;
    reason_ = RUNTIME_LIMIT;
    stoppedAt_ = now;
    return;
  }

  if (!freshCurrent) return;
  if (!isfinite(current) || current < 0.0) {
    state_ = FAULT;
    reason_ = SENSOR_FAILURE;
    stoppedAt_ = now;
    return;
  }

  const uint32_t intervalMs = now - lastIntegrationAt_;
  if (intervalMs > 1000) {
    state_ = FAULT;
    reason_ = SENSOR_FAILURE;
    stoppedAt_ = now;
    return;
  }

  const double power = voltage * current;
  if (previousSampleValid_) {
    const double hours = intervalMs / 3600000.0;
    milliampHours_ += ((previousCurrent_ + current) * 0.5) * hours * 1000.0;
    wattHours_ += ((previousPower_ + power) * 0.5) * hours;
  }
  previousCurrent_ = current;
  previousPower_ = power;
  lastIntegrationAt_ = now;
  previousSampleValid_ = true;
}

void ElectronicLoadCapacityTest::stop(uint32_t now) {
  if (state_ == RUNNING) {
    state_ = STOPPED;
    reason_ = USER_STOP;
    stoppedAt_ = now;
  }
}

void ElectronicLoadCapacityTest::reset(uint32_t now) {
  state_ = IDLE;
  reason_ = NONE;
  startedAt_ = now;
  lastIntegrationAt_ = now;
  stoppedAt_ = now;
  milliampHours_ = 0.0;
  wattHours_ = 0.0;
  previousCurrent_ = 0.0;
  previousPower_ = 0.0;
  previousSampleValid_ = false;
}

ElectronicLoadCapacityTest::State ElectronicLoadCapacityTest::state() const {
  return state_;
}

ElectronicLoadCapacityTest::Reason ElectronicLoadCapacityTest::reason() const {
  return reason_;
}

uint32_t ElectronicLoadCapacityTest::elapsedMs(uint32_t now) const {
  if (state_ == RUNNING) return now - startedAt_;
  return stoppedAt_ - startedAt_;
}

double ElectronicLoadCapacityTest::milliampHours() const { return milliampHours_; }
double ElectronicLoadCapacityTest::wattHours() const { return wattHours_; }
bool ElectronicLoadCapacityTest::running() const { return state_ == RUNNING; }
