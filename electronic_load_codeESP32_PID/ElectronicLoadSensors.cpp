#include "ElectronicLoadSensors.h"
#include "ElectronicLoadConfig.h"

ElectronicLoadSensors::ElectronicLoadSensors(ADS1115 &adc)
  : adc_(adc), healthy_(false), pending_(false), currentValid_(false),
    channel_(0), requestedAt_(0), lastPollAt_(0), currentTimestamp_(0) {
  voltageValid_[0] = false;
  voltageValid_[1] = false;
  voltageTimestamp_[0] = 0;
  voltageTimestamp_[1] = 0;
}

bool ElectronicLoadSensors::begin(uint32_t now) {
  healthy_ = adc_.begin();
  if (!healthy_) return false;

  adc_.setGain(1);
  adc_.setMode(1);  // Preserve the existing single-shot measurement mode.
  adc_.setDataRate(ADS_DATA_RATE);
  channel_ = 0;
  return request(now);
}

bool ElectronicLoadSensors::request(uint32_t now) {
  adc_.requestADC(channel_);
  if (adc_.getError() != ADS1X15_OK) {
    fail();
    return false;
  }
  requestedAt_ = now;
  lastPollAt_ = now;
  pending_ = true;
  return true;
}

bool ElectronicLoadSensors::service(uint32_t now, Sample &sample) {
  if (!healthy_) return false;
  if (!pending_) {
    request(now);
    return false;
  }

  const uint32_t elapsed = now - requestedAt_;
  if (elapsed < ADS_CONVERSION_WAIT_MS || now - lastPollAt_ < 1) return false;
  lastPollAt_ = now;

  const bool ready = adc_.isReady();
  if (adc_.getError() != ADS1X15_OK) {
    fail();
    return false;
  }
  if (!ready) {
    if (elapsed >= ADS_CONVERSION_TIMEOUT_MS) fail();
    return false;
  }

  sample.channel = channel_;
  sample.raw = adc_.getValue();
  sample.timestamp = now;
  if (adc_.getError() != ADS1X15_OK) {
    fail();
    return false;
  }

  if (channel_ == 0) {
    currentValid_ = true;
    currentTimestamp_ = now;
  } else {
    const uint8_t voltageIndex = channel_ - 1;
    voltageValid_[voltageIndex] = true;
    voltageTimestamp_[voltageIndex] = now;
  }

  channel_ = (channel_ + 1) % 3;
  pending_ = false;
  return true;
}

bool ElectronicLoadSensors::healthy() const { return healthy_; }
bool ElectronicLoadSensors::currentValid() const { return currentValid_; }

bool ElectronicLoadSensors::voltageValid(uint8_t channel) const {
  return channel < 2 && voltageValid_[channel];
}

uint32_t ElectronicLoadSensors::currentTimestamp() const {
  return currentTimestamp_;
}

uint32_t ElectronicLoadSensors::voltageTimestamp(uint8_t channel) const {
  return channel < 2 ? voltageTimestamp_[channel] : 0;
}

void ElectronicLoadSensors::resetFault() {
  healthy_ = false;
  pending_ = false;
  currentValid_ = false;
  voltageValid_[0] = false;
  voltageValid_[1] = false;
}

void ElectronicLoadSensors::fail() {
  healthy_ = false;
  pending_ = false;
}
