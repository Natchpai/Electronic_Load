#pragma once

#include <stdint.h>
#include "ADS1X15.h"

class ElectronicLoadSensors {
public:
  struct Sample {
    uint8_t channel;
    int16_t raw;
    uint32_t timestamp;
  };

  explicit ElectronicLoadSensors(ADS1115 &adc);
  bool begin(uint32_t now);
  bool service(uint32_t now, Sample &sample);
  bool healthy() const;
  bool currentValid() const;
  bool voltageValid(uint8_t channel) const;
  uint32_t currentTimestamp() const;
  uint32_t voltageTimestamp(uint8_t channel) const;
  void resetFault();

private:
  ADS1115 &adc_;
  bool healthy_;
  bool pending_;
  bool currentValid_;
  bool voltageValid_[2];
  uint8_t channel_;
  uint32_t requestedAt_;
  uint32_t lastPollAt_;
  uint32_t currentTimestamp_;
  uint32_t voltageTimestamp_[2];

  bool request(uint32_t now);
  void fail();
};
