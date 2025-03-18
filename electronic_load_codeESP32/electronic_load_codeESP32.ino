#include <LiquidCrystal_I2C.h>

// GPIO34 ADC1
// GPIO35 ADC1
// GPIO32 ADC1
// GPIO33 ADC1
// GPIO25 ADC2
// GPIO26 ADC2

// Analog INPUT   : Current sense x1 , Voltage sense x2 , Temperature sense x1
// Digital INPUT  : Yellow_button x1, Red_button x1, Rotary_button x1, Rotary Encoder x1
// Digital OUTPUT : Current control with PWM 10KHz, Fan control
// Display        : LCD I2C Interface 16x4 

// ADC INPUT
//  Current sense     @ GPIO34
//  Voltage sense 1st @ GPIO35
//  Voltage sense 2nd @ GPIO32
//  Temperature sense @ GPIO33

// Digital INPUT
//  Yellow_button @ GPIO25
//  Red_button    @ GPIO16
//  Rotary_button @ GPIO27
//  Rotary Encoder  DT@ GPIO 

// Digital OUTPUT
//  PWM control @ GPIO14
//  Fan control @ GPIO2


LiquidCrystal_I2C lcd(0x27, 16, 4);
void setup() {
  // put your setup code here, to run once:
  lcd.begin();
  lcd.backlight();

}

void loop() {
  // put your main code here, to run repeatedly:

}
