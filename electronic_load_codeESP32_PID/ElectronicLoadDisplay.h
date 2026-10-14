#pragma once

#include <Arduino.h>
#include <stddef.h>

class ElectronicLoadDisplay {
  static const uint8_t columns = 20;
  static const uint8_t rows = 4;

  char desired[rows][columns];
  char displayed[rows][columns];
  bool displayedValid[rows][columns];
  uint8_t cursorColumn = 0;
  uint8_t cursorRow = 0;

  void put(char value);
  void putText(const char *text);

public:
  ElectronicLoadDisplay();

  void init();
  void backlight();
  void createChar(uint8_t index, uint8_t *glyph);
  void clear();
  void setCursor(uint8_t column, uint8_t row);
  size_t write(uint8_t value);
  size_t print(char value);
  size_t print(const char *value);
  size_t print(const String &value);

  template <typename T>
  size_t print(T value) {
    String text(value);
    return print(text);
  }

  size_t print(double value, int digits);
  void flush();
};

extern ElectronicLoadDisplay lcd;
