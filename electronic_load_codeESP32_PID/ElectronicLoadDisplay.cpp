#include "ElectronicLoadDisplay.h"
#include "LiquidCrystal_I2C.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

namespace {
LiquidCrystal_I2C lcdHardware(0x27, 20, 4);
}

ElectronicLoadDisplay lcd;

ElectronicLoadDisplay::ElectronicLoadDisplay() {
  for (uint8_t row = 0; row < rows; row++) {
    for (uint8_t column = 0; column < columns; column++) {
      desired[row][column] = ' ';
      displayed[row][column] = '\0';
      displayedValid[row][column] = false;
    }
  }
}

void ElectronicLoadDisplay::put(char value) {
  if (cursorColumn < columns && cursorRow < rows) {
    desired[cursorRow][cursorColumn] = value;
  }
  if (cursorColumn < columns) cursorColumn++;
}

void ElectronicLoadDisplay::putText(const char *text) {
  if (text == nullptr) return;
  while (*text != '\0') put(*text++);
}

void ElectronicLoadDisplay::init() { lcdHardware.init(); }
void ElectronicLoadDisplay::backlight() { lcdHardware.backlight(); }
void ElectronicLoadDisplay::createChar(uint8_t index, uint8_t *glyph) {
  lcdHardware.createChar(index, glyph);
}

void ElectronicLoadDisplay::clear() {
  for (uint8_t row = 0; row < rows; row++) {
    for (uint8_t column = 0; column < columns; column++) desired[row][column] = ' ';
  }
  cursorColumn = 0;
  cursorRow = 0;
}

void ElectronicLoadDisplay::setCursor(uint8_t column, uint8_t row) {
  cursorColumn = column;
  cursorRow = row;
}

size_t ElectronicLoadDisplay::write(uint8_t value) {
  put(static_cast<char>(value));
  return 1;
}

size_t ElectronicLoadDisplay::print(char value) {
  put(value);
  return 1;
}

size_t ElectronicLoadDisplay::print(const char *value) {
  if (value == nullptr) return 0;
  size_t length = strlen(value);
  putText(value);
  return length;
}

size_t ElectronicLoadDisplay::print(int value) {
  char buffer[16];
  const int length = snprintf(buffer, sizeof(buffer), "%d", value);
  return length > 0 ? print(buffer) : 0;
}

size_t ElectronicLoadDisplay::print(unsigned int value) {
  char buffer[16];
  const int length = snprintf(buffer, sizeof(buffer), "%u", value);
  return length > 0 ? print(buffer) : 0;
}

size_t ElectronicLoadDisplay::print(long value) {
  char buffer[24];
  const int length = snprintf(buffer, sizeof(buffer), "%ld", value);
  return length > 0 ? print(buffer) : 0;
}

size_t ElectronicLoadDisplay::print(unsigned long value) {
  char buffer[24];
  const int length = snprintf(buffer, sizeof(buffer), "%lu", value);
  return length > 0 ? print(buffer) : 0;
}

size_t ElectronicLoadDisplay::print(double value, int digits) {
  if (digits < 0) digits = 0;
  if (digits > 9) digits = 9;
  char buffer[40];
  dtostrf(value, 0, static_cast<unsigned int>(digits), buffer);
  return print(buffer);
}

void ElectronicLoadDisplay::flush() {
  for (uint8_t row = 0; row < rows; row++) {
    uint8_t column = 0;
    while (column < columns) {
      while (column < columns && displayedValid[row][column] &&
             displayed[row][column] == desired[row][column]) column++;
      if (column == columns) break;

      uint8_t start = column;
      lcdHardware.setCursor(start, row);
      while (column < columns &&
             (!displayedValid[row][column] || displayed[row][column] != desired[row][column])) {
        lcdHardware.write(static_cast<uint8_t>(desired[row][column]));
        displayed[row][column] = desired[row][column];
        displayedValid[row][column] = true;
        column++;
      }
    }
  }
}
