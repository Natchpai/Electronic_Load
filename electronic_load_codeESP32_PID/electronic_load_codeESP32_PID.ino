#include <math.h>

#include "ADS1X15.h"
#include "LiquidCrystal_I2C.h"
#include "BfButton.h"
#include "PID_v1.h"
// ADC INPUT
// Move to 16Bits ADC I2C bus at: 0x48
#define temp_sense_pin 34

#define buzzer_pin 32

// Digital INPUT
#define red_button_pin 26
#define yellow_button_pin 25
#define rotary_button_pin 27
#define Encoder_DT 13
#define Encoder_CLK 14

// Digital OUTPUT
#define PWM_pinOut 4
#define fanCtrl_pinOut 15
#define gateCtrl_pinOut 2

// Setting time
#define updateTemp_time 1000
#define updateSensor_time 500
#define updateVoltage_time 250

#define VIN_MIN 1.0

uint8_t sec = 0;
uint8_t minut = 0;
int timeMax = 30;
int timesetMax = 60;
int timesetmin = 0;
int TempsetMax = 125;
int Tempsetmin = 40;

uint32_t lastUpdateTempSensor;
uint32_t lastTime;
uint32_t LastupdateSensor;
uint32_t lastPressRedButton;
uint32_t lastBlink = 0;
uint32_t lastBlinkArrow = 0;

uint32_t clearDisplayFormMenu = 1;
bool drawSetValueFirstTime = 0;

bool encode_clk;
bool encode_dt;
bool rotaryIsNextTigger;

// Temp adj value
float beta = 3435;
float To = 298;
float R25 = 10000;
int8_t adj_temp = 0;
float CurrentTemp;
int FanTrig_TempUpper = 45;
int FanTrig_TempLower = 35;
bool FanTrig = false;
bool AutoFanMode = true;
int Temp_Max = 90;

// Parameter
bool onLoad = false;
bool loadFlag = 1;
uint32_t onLoad_time = 0;

double Iin_maximum = 20.00;

double IMax = 30.00;
double Imin = 00.00;
double Iset = 01.00;
double Iset_forCal = 0.00;

double PMax = 800.00;
double Pmin = 00.00;
double Pset = 0.00;

double RMax = 9999.9999;
double Rmin = 50.00;
double Rset = 50.00;

bool V_sense_number = 0; // Default at Input terminal
double V_sense_1st = 00.000;
double V_sense_2nd = 00.000;
double I_sense = 00.000;

double PowerOut = 0.000;
double Resistance = 0.000;
double V_sense_select = 0.000;

uint8_t select_digit = 3;

// Mode0 : Main menu
int menu = 1;
uint8_t frame = 1;
uint8_t mode = 0; // if 0 : Back to Main menu Auto
uint8_t previous_menu = 255;
uint8_t previous_mode = 255;

uint8_t digit_1 = 1;
uint8_t digit_2 = 2;
uint8_t digit_3 = 3;
int menuItemMax = 6;
int menuItemmin = 1;


//Define Variables we'll be connecting to
double Setpoint, Input, Output;

//Define the aggressive and conservative Tuning Parameters
// Don't touch Kd, it will be osscilate
double consKp=0.0, consKi=450, consKd=0.0;

double gainMax = 800.00;
double gainmin = 0.00;

PID calPID(&Input, &Output, &Setpoint, consKp, consKi, consKd, DIRECT);

String menuList[6] = {
  "Const Current     ",
  "Const Power       ",
  "Const Resistance  ",
  "Digital Voltmeter ",
  "Utility           ",
  "PID Tuner         ",
};

int menu_setting = 1;
int frame_setting = 1;
int cursor = 1;
int cursorMax = 2;
int cursormin = 1;


String settingList[6]= {
  "Current Max       ",
  "Volt sense        ",
  "Fan Trig          ",
  "Runtime Max       ",
  "Shutdown Temp     ",
  "                  ",
};

String onOffList[2] = {
  "ON                ",
  "OFF               "
};

String FanList[2] = {
  "Auto              ",
  "Manual            "
};


String PIDList[3]= {
  "Kp : ",
  "Ki :",
  "Kd : "
};

int cursorPIDMax = 3;
int cursorPIDmin = 1;
int menu_pid = 1;
int frame_pid = 1;


// Set PWM value
uint8_t resolution = 16;
uint16_t frequency = 1000;
uint16_t dutyCycle_Max = 65535;
uint16_t dutyCycle_min = 0;
int dutyCycle = 0;
float dutyFactor = 0;

// Analog to Digital Converter
ADS1115 ADS(0x48);
int16_t I_ADC ;
int16_t V0_ADC ;
int16_t V1_ADC ;

// LiquidCrystal set pin
LiquidCrystal_I2C lcd(0x27, 20, 4);

byte arrow[] = { // arrow >
  B01000,
  B01100,
  B01110,
  B01111,
  B01110,
  B01100,
  B01000,
  B00000
};

// byte subBar1[] = {B10000, B10000, B10000, B10000, B10000, B10000, B10000, B10000};
// byte subBar2[] = {B11000, B11000, B11000, B11000, B11000, B11000, B11000, B11000};
// byte subBar3[] = {B11100, B11100, B11100, B11100, B11100, B11100, B11100, B11100};
// byte subBar4[] = {B11110, B11110, B11110, B11110, B11110, B11110, B11110, B11110};
// byte subBar5[] = {B11111, B11111, B11111, B11111, B11111, B11111, B11111, B11111};

byte subBar1[] = {B00000, B00000, B10000, B10000, B10000, B10000, B10000, B10000};
byte subBar2[] = {B00000, B00000, B11000, B11000, B11000, B11000, B11000, B11000};
byte subBar3[] = {B00000, B00000, B11100, B11100, B11100, B11100, B11100, B11100};
byte subBar4[] = {B00000, B00000, B11110, B11110, B11110, B11110, B11110, B11110};
byte subBar5[] = {B00000, B00000, B11111, B11111, B11111, B11111, B11111, B11111};

// Button Fever
BfButton redButton(BfButton::STANDALONE_DIGITAL, red_button_pin, true, LOW); 
BfButton yellowButton(BfButton::STANDALONE_DIGITAL, yellow_button_pin, true, LOW); 
BfButton rotaryButton(BfButton::STANDALONE_DIGITAL, rotary_button_pin, false, LOW); 

void pressRed (BfButton *redButton, BfButton::press_pattern_t pattern) {
  switch (pattern) {
    case BfButton::SINGLE_PRESS:
      if(mode == 1) {
        onLoad = !onLoad;
      }
      break;
    case BfButton::DOUBLE_PRESS:
      break;
    case BfButton::LONG_PRESS:
      if(mode == 1) {
        onLoad = !onLoad;
        mode = 2;
      }
      break;
  }
}

void pressYellow (BfButton *yellowButton, BfButton::press_pattern_t pattern) {
  switch (pattern) {
    case BfButton::SINGLE_PRESS:
    // Back to previous
      if(mode != 0) mode = mode - 1;
      if(mode == 0) lcd.clear();
      
      break;
    case BfButton::DOUBLE_PRESS:
      
      break;
    case BfButton::LONG_PRESS:
      if(mode == 0) {
        mode = 1;
      }
      else if(mode == 1) {
        mode = 2;
      } 
      break;
  }
}

void pressRotary (BfButton *rotaryButton, BfButton::press_pattern_t pattern) {
  switch (pattern) {
    case BfButton::SINGLE_PRESS: 
      if(mode == 0) {
        mode = 1;
      }
      else if(mode == 1) {
        mode = 2;
      }  
      else if(mode == 2) {
        switch (menu) {
        case 1:
          if (select_digit != 4) {
            select_digit++;
          } else {
            select_digit = 0;
          }
          break;
        case 2:
          if (select_digit != 5) {
            select_digit++;
          } else {
            select_digit = 0;
          }
          break;
        case 3:
          if (select_digit != 6) {
            select_digit++;
          } else {
            select_digit = 0;
          }
          break;
        }
      }

      break;
    case BfButton::DOUBLE_PRESS:
      break;
    case BfButton::LONG_PRESS:
      break;
  }
}

////////////////////////////////////////// ROTARY ENCODER ///////////////////////////////////////
void printBegin() {
  lcd.setCursor(1, 1);
  lcd.print("DC Electronic Loads");
  lcd.setCursor(8, 3);
  lcd.print("2025");
  delay(3000);
}

void count_trig_float(double* value, uint8_t* digit, double* Max, double* min, bool up = true) { // digit 4 = X_.___, digit 1 = __._X_
  if(up == true) {
    switch (*digit) {
      case 0:
        if((*value + 0.001) > *Max) { *value = *Max; }
        else {*value = *value + 0.001;}
        break;
      case 1:
        if((*value + 0.01) > *Max) { *value = *Max; }
        else {*value = *value + 0.01;}
        break;
      case 2:
        if((*value + 0.1) > *Max) { *value = *Max; }
        else {*value = *value + 0.1;}
        break;
      case 3:
        if((*value + 1.0) > *Max) { *value = *Max; }
        else {*value = *value + 1.0;}
        break;
      case 4:
        if(((*value + 10.0) > *Max)) {}//{ *digits = *Max; }
        else {*value = *value + 10.0;}
        break;
      case 5:
        if(((*value + 100.0) > *Max)) {}//{ *digits = *Max; }
        else {*value = *value + 100.0;}
        break;
      case 6:
        if(((*value + 1000.0) > *Max)) {}//{ *digits = *Max; }
        else {*value = *value + 1000.0;}
        break;
      case 7:
        if(((*value + 10000.0) > *Max)) {}//{ *digits = *Max; }
        else {*value = *value + 10000.0;}
        break;
    }
  }
  else {
    switch (*digit) {
      case 0:
        if((*value - 0.001) < *min) { *value = *min; }
        else {*value = *value - 0.001;}
        break;
      case 1:
        if((*value - 0.01) < *min) { *value = *min; }
        else {*value = *value - 0.01;}
        break;
      case 2:
        if((*value - 0.1) < *min) { *value = *min; }
        else {*value = *value - 0.1;}
        break;
      case 3:
        if((*value - 1.0) < *min) {}//{ *value = *min; }
        else {*value = *value - 1.0;}
        break;
      case 4:
        if(((*value - 10.0) < *min)) {}//{ *value = *min; }
        else {*value = *value - 10.0;}
        break;
      case 5:
        if(((*value - 100.0) < *min)) {}//{ *value = *min; }
        else {*value = *value - 100.0;}
        break;
      case 6:
        if(((*value - 1000.0) < *min)) {}//{ *value = *min; }
        else {*value = *value - 1000.0;}
        break;
      case 7:
        if(((*value - 10000.0) < *min)) {}//{ *value = *min; }
        else {*value = *value - 10000.0;}
        break;  
      
    }
  }
}

void count_trig_int(int* value, uint8_t* digit, int* Max, int* min, bool up = true) { // digit 4 = X_.___, digit 1 = __._X_
  if(up == true) {
    switch (*digit) {
      case 1:
        if((*value + 1) > *Max) { *value = *Max; }
        else {*value = *value + 1;}
        break;
      case 2:
        if(((*value + 10.0) > *Max)) {}//{ *digits = *Max; }
        else {*value = *value + 10;}
        break;
    }
  }
  else {
    switch (*digit) {
      case 1:
        if((*value - 1) < *min) {}//{ *value = *min; }
        else {*value = *value - 1;}
        break;
      case 2:
        if(((*value - 10.0) < *min)) {}//{ *value = *min; }
        else {*value = *value - 10;}
        break;
    }
  }
}

void count(bool state = true) { // state is true : countup
  if(mode == 0) {
    // score main menu
    count_trig_int(&menu, &digit_1, &menuItemMax, &menuItemmin, !state);
  } else 
  
  if(mode == 2) {
    if(menu == 1) count_trig_float(&Iset, &select_digit, &IMax, &Imin, state);
    else if(menu == 2) count_trig_float(&Pset, &select_digit, &PMax, &Pmin, state);
    else if(menu == 3) count_trig_float(&Rset, &select_digit, &RMax, &Rmin, state);

    else if(menu == 5) {
      switch(menu_setting) {
        case 1:
          count_trig_float(&Iin_maximum, &select_digit, &IMax, &Imin, state);
          break;
        case 2:
          count_trig_int(&cursor, &digit_1, &cursorMax, &cursormin, !state);
          if(cursor == 1) V_sense_number = 0;
          else V_sense_number = 1;
          break;
        case 3:
          count_trig_int(&cursor, &digit_1, &cursorMax, &cursormin, !state);
          if(cursor == 1) AutoFanMode = true;
          else AutoFanMode = false;
          break;
        case 4:
          count_trig_int(&timeMax, &digit_1, &timesetMax, &timesetmin, state);
          break;
        case 5:
          count_trig_int(&Temp_Max, &digit_1, &TempsetMax, &Tempsetmin, state);
          break;
      }
    }

    else if(menu == 6) {
      if(menu_pid == 1) count_trig_float(&consKp, &digit_2, &gainMax, &gainmin, state);
      else if(menu_pid == 2) count_trig_float(&consKi, &digit_3, &gainMax, &gainmin, state);
      else if(menu_pid == 3) count_trig_float(&consKd, &digit_2, &gainMax, &gainmin, state);
      
    }

  } else 

  if(mode == 1) {
    if(menu == 5) count_trig_int(&menu_setting, &digit_1, &menuItemMax, &menuItemmin, !state);
    else if(menu == 6) count_trig_int(&menu_pid, &digit_1, &cursorPIDMax, &cursormin, !state);
  }
}

void rotaryEncode() {
  encode_clk = digitalRead(Encoder_CLK);
  encode_dt = digitalRead(Encoder_DT);
  if (encode_clk == 0 && encode_dt == 1 && rotaryIsNextTigger == 1) {
    rotaryIsNextTigger = 0;
    count(1);
    digitalWrite(buzzer_pin, HIGH); 
  } 
  else if(encode_dt == 0 && encode_clk == 1 && rotaryIsNextTigger == 1) {
    rotaryIsNextTigger = 0;
    count(0);
    digitalWrite(buzzer_pin, HIGH); 
  } 
  else if(encode_clk == 1 && encode_dt == 1 ) {
    rotaryIsNextTigger = 1;
    digitalWrite(buzzer_pin, LOW);
  }
}
////////////////////////////////////////// ROTARY ENCODER ///////////////////////////////////////

void readTemp() {
  float v_ntc = (analogRead(temp_sense_pin) * 3.3) / 4095;
  float R_ntc = ((3.3* 11000) / v_ntc) - 11000;
  CurrentTemp = ( (beta) / ( log(R_ntc/R25) + (beta/To)) ) - 273 + 4 + adj_temp;
}

void updateTempSensor() {
  // Tempurature sensor
  readTemp();
  if((int)CurrentTemp <= 99) {
    lcd.setCursor(15, 0);
    lcd.print(" ");
    lcd.setCursor(16, 0);
    lcd.print((int)CurrentTemp);
    lcd.setCursor(18, 0);
    lcd.print(char(223));
    lcd.setCursor(19, 0);
    lcd.print("C");
  }else{
    lcd.setCursor(15, 0);
    lcd.print((int)CurrentTemp);
    lcd.setCursor(18, 0);
    lcd.print(char(223));
    lcd.setCursor(19, 0);
    lcd.print("C");
  }
}

void readVoltageAll() {
  V0_ADC = ADS.readADC(1);  
  V1_ADC = ADS.readADC(2); 
  if (ADS.getError() == ADS1X15_OK)
  { 
    V_sense_1st = abs(V0_ADC * 0.001250);
    V_sense_2nd = abs(V1_ADC * 0.001250);
  }
}

void readSensor() {
  ADS.setGain(1); // Verf = 4.096 > Factor = 125uV / Bit;

  I_ADC = ADS.readADC(0);  
  V0_ADC = ADS.readADC(1);  
  V1_ADC = ADS.readADC(2);  

  if (ADS.getError() == ADS1X15_OK)
  { 

    I_sense = abs(I_ADC * 0.001250);
    if(V_sense_number == 0) {
      V_sense_select = abs(V0_ADC * 0.001250);
    }
    else {
      V_sense_select = abs(V1_ADC * 0.001250);
    }
  }

  // Power calculate
  PowerOut = I_sense * V_sense_select;

  // Resistance calculate
  if(I_sense != 0.0001) {
    Resistance = V_sense_select / I_sense;
  } else { Resistance = 1000000.0;}
  

  if(menu == 1 && V_sense_select >= VIN_MIN) {
    Iset_forCal  = Iset;
  }
  // if activate = 0 : PWM duty = 0
  else if(menu == 2 && V_sense_select >= VIN_MIN) {// Protection
    Iset_forCal  = Pset / V_sense_select;
  }
  else if(menu == 3 && Rset > 0.01) {// devide by zero
    Iset_forCal  = V_sense_select / Rset;
  }
  
  // Protection
  // if(Iset_forCal  > Iin_maximum) Iset_forCal = 1.0;
  if(V_sense_select < VIN_MIN) {
    Iset_forCal = 0;
  }

}

void updateSensor() {
  if( (menu == 1 ||  menu == 2 || menu == 3) && mode != 0 ) {
    if(millis() - lastUpdateTempSensor >= updateTemp_time) {
      lastUpdateTempSensor = millis();
      updateTempSensor();
    }

    if(millis() - LastupdateSensor >= updateSensor_time)
    {
      LastupdateSensor = millis();
    // Voltage display
      if(V_sense_select < 10.000) 
      {
        lcd.setCursor(2, 1);
        lcd.print(" ");
        lcd.setCursor(3, 1);
        lcd.print(V_sense_select, 3);
        lcd.setCursor(8, 1);
        lcd.print("V");
      }
      else 
      {
        lcd.setCursor(2, 1);
        lcd.print(V_sense_select, 3);
        lcd.setCursor(8, 1);
        lcd.print("V");
      }

    // Current Display 
      if(I_sense < 10.000) 
      {
        lcd.setCursor(11, 1);
        lcd.print(" ");
        lcd.setCursor(12, 1);
        lcd.print(I_sense, 3);
        lcd.setCursor(17, 1);
        lcd.print("A");
      }
      else 
      {
        lcd.setCursor(11, 1);
        lcd.print(I_sense, 3);
        lcd.setCursor(17, 1);
        lcd.print("A");
      }
      
    // Power Display 
      
      if(PowerOut < 10.000) 
      {
        lcd.setCursor(1, 2);
        lcd.print("  ");
        lcd.setCursor(3, 2);
        lcd.print(PowerOut, 3);
        lcd.setCursor(8, 2);
        lcd.print("W");
      }

      else if(PowerOut < 100.000) 
      {
        lcd.setCursor(1, 2);
        lcd.print(" ");
        lcd.setCursor(2, 2);
        lcd.print(PowerOut, 3);
        lcd.setCursor(8, 2);
        lcd.print("W");
      }
      else 
      {
        lcd.setCursor(1, 2);
        lcd.print(PowerOut, 3);
        lcd.setCursor(8, 2);
        lcd.print("W");
      }

    // Resistance Display

      if(Resistance< 10.0) 
      {
        lcd.setCursor(10, 2);
        lcd.print("  ");
        lcd.setCursor(12, 2);
        lcd.print(Resistance, 3);
        lcd.setCursor(17, 2);
        lcd.print(char(244));
        lcd.print(" ");
      }
      else if(Resistance < 100.0) 
      { 
        lcd.setCursor(10, 2);
        lcd.print(" ");
        lcd.setCursor(11, 2);
        lcd.print(Resistance, 3);
        lcd.setCursor(17, 2);
        lcd.print(char(244));
        lcd.print(" ");
      }
      else if(Resistance < 1000.0)
      {
        lcd.setCursor(10, 2);
        lcd.print(Resistance, 3);
        lcd.setCursor(17, 2);
        lcd.print(char(244));
        lcd.print(" ");
      }
      else if(Resistance < 10000.0){
        lcd.setCursor(10, 2);
        lcd.print("  ");
        lcd.setCursor(12, 2);
        lcd.print( (Resistance/1000.0) , 3);
        lcd.setCursor(17, 2);
        lcd.print("k");
        lcd.setCursor(18, 2);
        lcd.print(char(244));
        lcd.print(" ");
      } else {
        lcd.setCursor(10, 2);
        lcd.print("  inf  ");
        lcd.setCursor(17, 2);
        lcd.print(char(244));
        lcd.print(" ");
      }
    }
  }
}

void updateRunMode() {
  if(mode != 0) {
    if(onLoad == 1 && (millis() - lastPressRedButton >= 500)) {
      lastPressRedButton = millis();
      lcd.setCursor(15, 3);
      lcd.print(minut / 10);
      lcd.print(minut % 10);
      lcd.print(":");
      lcd.print(sec / 10);
      lcd.print(sec % 10);
    }
    if(onLoad == 0) {
      if(millis() - lastBlinkArrow >= 500) {
        lcd.setCursor(15, 3);
        lcd.print("  OFF");
      }
      if(millis() - lastBlinkArrow >= 1000) {
        lastBlinkArrow = millis();
        lcd.setCursor(15, 3);
        lcd.print(" ");
        lcd.write(0); // arrow
      }
      sec = 0; minut = 0; 
    }
  } 
  else 
  {
    sec = 0; minut = 0; 
  }
  digitalWrite(gateCtrl_pinOut, onLoad);
}

int lastNumSubBars1;
int lastNumSubBars2;

void printBar(float data, float maxData, int maxBars, uint8_t column, uint8_t row, int *lastNumSubBars) {

  int numSubBars = map(data, 0, maxData, 0, maxBars);

  if (numSubBars < *lastNumSubBars) {
    for (int i = numSubBars / 5 ; i < (*lastNumSubBars / 5) + 1; i++) {
        lcd.setCursor(i + column, row);
        lcd.print(" "); 
    }
  }

  lcd.setCursor(column, row);
  for (int i = 0; i < numSubBars / 5; i++) {
      lcd.write(5); 
  }

  int remainder = numSubBars % 5;
  if (remainder > 0) {
      lcd.write(remainder);
  }

  *lastNumSubBars = numSubBars;

}

void drawSetValue_setMode(float value, String txt, uint8_t digit) {
  lcd.setCursor(0, 3);
  lcd.print("Set");
  if(digit < 3) {
    if(value < 10.00) {
      lcd.setCursor(4, 3);
      lcd.print("0");
      lcd.setCursor(5, 3);
      lcd.print(value, 3);
      lcd.setCursor(10, 3);
      lcd.print(txt);
    } 
    else {
      lcd.setCursor(4, 3);
      lcd.print(value, 3);
      lcd.setCursor(10, 3);
      lcd.print(txt);
    }
  }
  else if(digit == 3) {
    if(value < 10.00) {
      lcd.setCursor(4, 3);
      lcd.print("00");
      lcd.setCursor(6, 3);
      lcd.print(value, 3);
      lcd.setCursor(11, 3);
      lcd.print(txt);
    } 
    else if(value < 100.00){
      lcd.setCursor(4, 3);
      lcd.print("0");
      lcd.setCursor(5, 3);
      lcd.print(value, 3);
      lcd.setCursor(11, 3);
      lcd.print(txt);
    }else{
      lcd.setCursor(4, 3);
      lcd.print(value, 3);
      lcd.setCursor(11, 3);
      lcd.print(txt);
    }
  }
  else if(digit == 4) {
    if(value < 10.00) {
      lcd.setCursor(4, 3);
      lcd.print("000");
      lcd.setCursor(7, 3);
      lcd.print(value, 3);
      lcd.setCursor(12, 3);
      lcd.print(txt);
    } 
    else if(value < 100.00){
      lcd.setCursor(4, 3);
      lcd.print("00");
      lcd.setCursor(6, 3);
      lcd.print(value, 3);
      lcd.setCursor(12, 3);
      lcd.print(txt);
    }
    else if(value < 1000.00){
      lcd.setCursor(4, 3);
      lcd.print("0");
      lcd.setCursor(5, 3);
      lcd.print(value, 3);
      lcd.setCursor(12, 3);
      lcd.print(txt);
    }
    else {
      lcd.setCursor(4, 3);
      lcd.print(value, 3);
      lcd.setCursor(12, 3);
      lcd.print(txt);
    }
  }
}

void drawSetValue_RunMode(float value, String txt) {
  lcd.setCursor(0, 3);
  lcd.print("Set");
  if(value < 10.00) {
    lcd.setCursor(4, 3);
    lcd.print(value, 3);
    // lcd.setCursor(12, 3);
    lcd.print(txt);
  } 
  else if(value < 100.00){
    lcd.setCursor(4, 3);
    lcd.print(value, 3);
    // lcd.setCursor(11, 3);
    lcd.print(txt);
  }
  else if(value < 1000.00){
    lcd.setCursor(4, 3);
    lcd.print(value, 3);
    // lcd.setCursor(12, 3);
    lcd.print(txt);
  }
  else {
    lcd.setCursor(4, 3);
    lcd.print((value/1000), 3);
    lcd.setCursor(12, 3);
    lcd.print("K");
    // lcd.setCursor(13, 3);
    lcd.print(txt);
  }
}

void menu0_MainMenu() {
  lcd.setCursor(0, 0);
  lcd.print(char(0xff));
  lcd.setCursor(1, 0);
  lcd.print("Select Function");

  rotaryEncode();
  rotaryButton.read();

  if(menu - frame == 3) {
    frame++;
  }

  else if(frame - menu == 1) {
    frame--;
  }
  
  lcd.setCursor(0, menu - frame + 1);
  lcd.write(0); 

  if (menu - frame == 0) {
    lcd.setCursor(0, 2);
    lcd.print(" "); 
    lcd.setCursor(0, 3);
    lcd.print(" "); 
    rotaryEncode();

  } 
  else if (menu - frame == 2) {
    lcd.setCursor(0, 1);
    lcd.print(" "); 
    lcd.setCursor(0, 2);
    lcd.print(" ");
    rotaryEncode();

  }
  else {
    lcd.setCursor(0, 1);
    lcd.print(" "); 
    lcd.setCursor(0, 3);
    lcd.print(" "); 
    rotaryEncode();
  }

  lcd.setCursor(1, 1);
  lcd.print(menuList[frame - 1]);
  lcd.setCursor(1, 2);
  lcd.print(menuList[frame + 0]);
  lcd.setCursor(1, 3);
  lcd.print(menuList[frame + 1]);
  rotaryEncode();
}

void menu1_constCurrent() {
  // if mode = 1 : Pre-Run Mode
  // if mode = 2 : Set value Mode
  rotaryEncode();
  switch (mode) {
    case 1:    
      if(previous_mode != mode) {
        previous_mode = mode;
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(char(0xff));
        lcd.setCursor(1, 0);
        lcd.print("Const Current");
        drawSetValue_RunMode(Iset, "A");
      }
      // update sensor
      updateSensor();
      break;

    case 2:
      updateSensor();
      if(previous_mode != mode) {
        previous_mode = mode;
        lcd.clear();
        lcd.setCursor(0, 3);
        lcd.print("Set");
        lcd.setCursor(0, 0);
        lcd.print(char(0xff));
        lcd.setCursor(1, 0);
        lcd.print("I Setpoint");
        drawSetValue_RunMode(Iset, "A");
      }
      if((millis() - lastBlink) >= 500) {
        lastBlink = millis();
        rotaryEncode();
        drawSetValue_setMode(Iset, "A", 2);
      }
      if((millis() - lastBlink) >= 250) {//  select_digit
        rotaryEncode();
        switch (select_digit) {
          case 0:
            lcd.setCursor(9, 3);
            lcd.print(char(0xff));
            break;
          case 1:
            lcd.setCursor(8, 3);
            lcd.print(char(0xff));
            break;
          case 2:
            lcd.setCursor(7, 3);
            lcd.print(char(0xff));
            break;
          case 3:
            lcd.setCursor(5, 3);
            lcd.print(char(0xff));
            break;
          case 4:
            lcd.setCursor(4, 3);
            lcd.print(char(0xff));
            break;
        }
        
        break;
      }
      break;
  }
}

void menu2_constPower() {
  // if mode = 1 : Pre-Run Mode
  // if mode = 2 : Set value Mode
  rotaryEncode();
  switch (mode) {
    case 1:    
      if(previous_mode != mode) {
        previous_mode = mode;
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(char(0xff));
        lcd.setCursor(1, 0);
        lcd.print("Const Power");
        drawSetValue_RunMode(Pset, "W");
      }

      if(millis() - lastUpdateTempSensor >= updateTemp_time) {
        lastUpdateTempSensor = millis();
        updateTempSensor();
      }
      break;
    case 2:
      if(previous_mode != mode) {
        previous_mode = mode;
        lcd.clear();
        lcd.setCursor(0, 3);
        lcd.print("Set");
        lcd.setCursor(0, 0);
        lcd.print(char(0xff));
        lcd.setCursor(1, 0);
        lcd.print("Power Setpoint");
        drawSetValue_RunMode(Pset, "W");
      }

      if((millis() - lastBlink) >= 400) {
        lastBlink = millis();
        rotaryEncode();
        drawSetValue_setMode(Pset, "W", 3);
      }
      if((millis() - lastBlink) >= 200) {//  select_digit
        rotaryEncode();
        switch (select_digit) {
          case 0:
            lcd.setCursor(10, 3);
            lcd.print(char(0xff));
            break;
          case 1:
            lcd.setCursor(9, 3);
            lcd.print(char(0xff));
            break;
          case 2:
            lcd.setCursor(8, 3);
            lcd.print(char(0xff));
            break;
          case 3:
            lcd.setCursor(6, 3);
            lcd.print(char(0xff));
            break;
          case 4:
            lcd.setCursor(5, 3);
            lcd.print(char(0xff));
            break;
          case 5:
            lcd.setCursor(4, 3);
            lcd.print(char(0xff));
            break;
        }
        
        break;
      }
      break;
  }
}

void menu3_constResistance() {
  // if mode = 1 : Pre-Run Mode
  // if mode = 2 : Set value Mode
  rotaryEncode();
  switch (mode) {
    case 1:    
      if(previous_mode != mode) {
        previous_mode = mode;
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(char(0xff));
        lcd.setCursor(1, 0);
        lcd.print("Constant R.");
        drawSetValue_RunMode(Rset, String(char(244)) );
      }

      if(millis() - lastUpdateTempSensor >= updateTemp_time) {
        lastUpdateTempSensor = millis();
        updateTempSensor();
      }
      break;
    case 2:
      if(previous_mode != mode) {
        previous_mode = mode;
        lcd.clear();
        lcd.setCursor(0, 3);
        lcd.print("Set");
        lcd.setCursor(0, 0);
        lcd.print(char(0xff));
        lcd.setCursor(1, 0);
        lcd.print("R. Setpoint");
        drawSetValue_RunMode(Rset, String(char(244)) );
      }

      if((millis() - lastBlink) >= 400) {
        lastBlink = millis();
        rotaryEncode();
        drawSetValue_setMode(Rset, String(char(244)) , 4);
      }
      if((millis() - lastBlink) >= 200) {//  select_digit
        rotaryEncode();
        switch (select_digit) {
          case 0:
            lcd.setCursor(11, 3);
            lcd.print(char(0xff));
            break;
          case 1:
            lcd.setCursor(10, 3);
            lcd.print(char(0xff));
            break;
          case 2:
            lcd.setCursor(9, 3);
            lcd.print(char(0xff));
            break;
          case 3:
            lcd.setCursor(7, 3);
            lcd.print(char(0xff));
            break;
          case 4:
            lcd.setCursor(6, 3);
            lcd.print(char(0xff));
            break;
          case 5:
            lcd.setCursor(5, 3);
            lcd.print(char(0xff));
            break;
          case 6:
            lcd.setCursor(4, 3);
            lcd.print(char(0xff));
            break;
        }
        
        break;
      }
      break;
  }
}

void menu4_VDM() {
  // if mode = 1 : Pre-Run Mode
  // if mode = 2 : Set value Mode
  // rotaryEncode();
  switch (mode) {
    case 1:    
      if(previous_mode != mode) {
        previous_mode = mode;
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(char(0xff));
        lcd.setCursor(1, 0);
        lcd.print("Voltmeter");
        lcd.setCursor(1, 1);
        lcd.print("CH0");
        lcd.setCursor(11, 1);
        lcd.print("CH1");
      }

      if(millis() - LastupdateSensor >= updateVoltage_time)
      {
        LastupdateSensor = millis();

        readVoltageAll();
      // Voltage display

        if(V_sense_1st < 10.000) 
        {
          lcd.setCursor(1, 2);
          lcd.print("0");
          lcd.setCursor(2, 2);
          lcd.print(V_sense_1st, 4);
          lcd.setCursor(9, 2);
          lcd.print("V");
        }
        else 
        {
          lcd.setCursor(1, 2);
          lcd.print(V_sense_1st, 4);
          lcd.setCursor(9, 2);
          lcd.print("V");
        }

        

        if(V_sense_2nd < 10.000) 
        {
          lcd.setCursor(11, 2);
          lcd.print("0");
          lcd.setCursor(12, 2);
          lcd.print(V_sense_2nd, 4);
          lcd.setCursor(19, 2);
          lcd.print("V");
        }
        else 
        {
          lcd.setCursor(11, 2);
          lcd.print(V_sense_2nd, 4);
          lcd.setCursor(19, 2);
          lcd.print("V");
        }

        printBar(V_sense_1st, 30.0, 42, 1, 3, &lastNumSubBars1);
        printBar(V_sense_2nd, 30.0, 42, 11, 3, &lastNumSubBars2);

      }

      if(millis() - lastBlinkArrow >= 500) {
        lcd.setCursor(15, 0);
        lcd.print("  RUN");
      }
      if(millis() - lastBlinkArrow >= 1000) {
        lastBlinkArrow = millis();
        lcd.setCursor(15, 0);
        lcd.print(" ");
        lcd.write(0); // arrow
      }

      break;
    case 2:
      if(previous_mode != mode) {
        previous_mode = mode;
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("NOTHING");
      }

      // if((millis() - lastBlink) >= 400) {
      //   lastBlink = millis();
      //   rotaryEncode();
      //   drawSetValue_setMode(Rset, String(char(244)) , 4);
      // }
      // if((millis() - lastBlink) >= 200) {//  select_digit
      //   rotaryEncode();
      //   switch (select_digit) {
      //     case 0:
      //       lcd.setCursor(11, 3);
      //       lcd.print(char(0xff));
      //       break;
      //     case 1:
      //       lcd.setCursor(10, 3);
      //       lcd.print(char(0xff));
      //       break;
      //     case 2:
      //       lcd.setCursor(9, 3);
      //       lcd.print(char(0xff));
      //       break;
      //     case 3:
      //       lcd.setCursor(7, 3);
      //       lcd.print(char(0xff));
      //       break;
      //     case 4:
      //       lcd.setCursor(6, 3);
      //       lcd.print(char(0xff));
      //       break;
      //     case 5:
      //       lcd.setCursor(5, 3);
      //       lcd.print(char(0xff));
      //       break;
      //     case 6:
      //       lcd.setCursor(4, 3);
      //       lcd.print(char(0xff));
      //       break;
      //   }
        
      //   break;
      // }
      break;
  }
}

void menu5_setting() {
  // if mode = 1 : Pre-Run Mode
  // if mode = 2 : Set value Mode
  // rotaryEncode();
  switch (mode) {
    case 1:    
      if(previous_mode != mode) {
        previous_mode = mode;
        lcd.clear();

      lcd.setCursor(0, 0);
      lcd.print(char(0xff));
      lcd.setCursor(1, 0);
      lcd.print("Utility");
      }

      rotaryEncode();
      rotaryButton.read();

      if(menu_setting - frame_setting == 3) {
        frame_setting++;
      }

      else if(frame_setting - menu_setting == 1) {
        frame_setting--;
      }
      
      lcd.setCursor(0, menu_setting - frame_setting + 1);
      lcd.write(0); 

      if (menu_setting - frame_setting == 0) {
        lcd.setCursor(0, 2);
        lcd.print(" "); 
        lcd.setCursor(0, 3);
        lcd.print(" "); 
        rotaryEncode();

      } 
      else if (menu_setting - frame_setting == 2) {
        lcd.setCursor(0, 1);
        lcd.print(" "); 
        lcd.setCursor(0, 2);
        lcd.print(" ");
        rotaryEncode();

      }
      else {
        lcd.setCursor(0, 1);
        lcd.print(" "); 
        lcd.setCursor(0, 3);
        lcd.print(" "); 
        rotaryEncode();
      }

      lcd.setCursor(1, 1);
      lcd.print(settingList[frame_setting - 1]);
      lcd.setCursor(1, 2);
      lcd.print(settingList[frame_setting + 0]);
      lcd.setCursor(1, 3);
      lcd.print(settingList[frame_setting + 1]);
      rotaryEncode();
      break;
    case 2:
      if(previous_mode != mode) {
        previous_mode = mode;
        lcd.clear();
      }
      lcd.setCursor(0, 0);
      lcd.print(char(0xff));
      lcd.setCursor(1, 0);
      lcd.print(settingList[menu_setting - 1]);

      switch(menu_setting) {
        case 1: // Set Current Max 
          lcd.setCursor(0, 1); lcd.print("IMax : ");
          if(Iin_maximum < 10) {
            lcd.setCursor(7, 1); 
            lcd.print("0");
            lcd.setCursor(8, 1); 
            lcd.print(Iin_maximum, 1);
          }else{
            lcd.setCursor(7, 1); 
            lcd.print(Iin_maximum, 1);
          }
          lcd.setCursor(11, 1); 
          lcd.print("A");
          break;
        case 2: // Select Vsense
          lcd.setCursor(0, cursor - 1 + 1);
          lcd.write(0); 
          if (cursor - 1 == 0) {
            lcd.setCursor(0, 2);
            lcd.print(" "); 
            lcd.setCursor(0, 3);
            lcd.print(" "); 
            rotaryEncode();

          } 
          else if (cursor - 1 == 2) {
            lcd.setCursor(0, 1);
            lcd.print(" "); 
            lcd.setCursor(0, 2);
            lcd.print(" ");
            rotaryEncode();

          }
          else {
            lcd.setCursor(0, 1);
            lcd.print(" "); 
            lcd.setCursor(0, 3);
            lcd.print(" "); 
            rotaryEncode();
          }
          lcd.setCursor(1, 1);
          lcd.print("V0 : Input");
          lcd.setCursor(1, 2);
          lcd.print("V1 : VOLT");
          rotaryEncode();
          break;
        case 3: // Select Fan mode
          lcd.setCursor(0, cursor - 1 + 1);
          lcd.write(0); 
          if (cursor - 1 == 0) {
            lcd.setCursor(0, 2);
            lcd.print(" "); 
            lcd.setCursor(0, 3);
            lcd.print(" "); 
            rotaryEncode();

          } 
          else if (cursor - 1 == 2) {
            lcd.setCursor(0, 1);
            lcd.print(" "); 
            lcd.setCursor(0, 2);
            lcd.print(" ");
            rotaryEncode();

          }
          else {
            lcd.setCursor(0, 1);
            lcd.print(" "); 
            lcd.setCursor(0, 3);
            lcd.print(" "); 
            rotaryEncode();
          }
          lcd.setCursor(1, 1);
          lcd.print(FanList[0]);
          lcd.setCursor(1, 2);
          lcd.print(FanList[1]);
          rotaryEncode();
          break;
        case 4: // Set Runtime Max 
          lcd.setCursor(0, 1); lcd.print("Runtime Max: ");
          if(timeMax < 10) {
            lcd.setCursor(13, 1); 
            lcd.print("0");
            lcd.setCursor(14, 1); 
            lcd.print(timeMax);
          }else{
            lcd.setCursor(13, 1); 
            lcd.print(timeMax);
          }
          lcd.setCursor(15, 1); 
          lcd.print("min");
          break;
        case 5: // Set Temp shutdown
          lcd.setCursor(0, 1); lcd.print("Max:");
          if(Temp_Max <= 99) {
            lcd.setCursor(4, 1); 
            lcd.print(" ");
            lcd.setCursor(5, 1); 
            lcd.print(Temp_Max);
          }else{
            lcd.setCursor(4, 1); 
            lcd.print(Temp_Max);
          }
          lcd.setCursor(7, 1); 
          lcd.print(char(223));
          lcd.setCursor(8, 1);
          lcd.print("C");
          break;
      }
      break;
  }
}

void menu6_PID() {
  // if mode = 1 : Pre-Run Mode
  // if mode = 2 : Set value Mode
  rotaryEncode();
  switch (mode) {
    case 1:    
      if(previous_mode != mode) {
        previous_mode = mode;
        lcd.clear();

        lcd.setCursor(0, 0);
        lcd.print(char(0xff));
        lcd.setCursor(1, 0);
        lcd.print("PID Tuner");
      }

      rotaryEncode();
      rotaryButton.read();
      
      lcd.setCursor(0, menu_pid - frame_pid + 1);
      lcd.write(0); 

      if (menu_pid - frame_pid == 0) {
        lcd.setCursor(0, 2);
        lcd.print(" "); 
        lcd.setCursor(0, 3);
        lcd.print(" "); 
        rotaryEncode();

      } 
      else if (menu_pid - frame_pid == 2) {
        lcd.setCursor(0, 1);
        lcd.print(" "); 
        lcd.setCursor(0, 2);
        lcd.print(" ");
        rotaryEncode();

      }
      else {
        lcd.setCursor(0, 1);
        lcd.print(" "); 
        lcd.setCursor(0, 3);
        lcd.print(" "); 
        rotaryEncode();
      }

      rotaryEncode();
      lcd.setCursor(1, 1);
      lcd.print(PIDList[frame_pid - 1]);
      lcd.setCursor(7, 1);
      lcd.print(consKp, 1);
      lcd.setCursor(1, 2);
      lcd.print(PIDList[frame_pid + 0]);

      if(consKi < 9.99) {
        lcd.setCursor(5, 2);
        lcd.print("  ");
        lcd.setCursor(7, 2);
        lcd.print(consKi, 1);
      }
      else if(consKi < 99.99){
        lcd.setCursor(5, 2);
        lcd.print(" ");
        lcd.setCursor(6, 2);
        lcd.print(consKi, 1);
      } else {
        lcd.setCursor(5, 2);
        lcd.print(consKi, 1);
      }

      lcd.setCursor(1, 3);
      lcd.print(PIDList[frame_pid + 1]);
      lcd.setCursor(7, 3);
      lcd.print(consKd, 1);
      rotaryButton.read();
      break; 

    case 2:
      if((millis() - lastBlink) >= 500) {
        lastBlink = millis();
        rotaryEncode();
        lcd.setCursor(1, 1);
        lcd.print(PIDList[frame_pid - 1]);
        lcd.setCursor(7, 1);
        lcd.print(consKp, 1);
        lcd.setCursor(1, 2);
        lcd.print(PIDList[frame_pid + 0]);

        if(consKi < 9.99) {
          lcd.setCursor(5, 2);
          lcd.print("  ");
          lcd.setCursor(7, 2);
          lcd.print(consKi, 1);
        }
        else if(consKi < 99.99){
          lcd.setCursor(5, 2);
          lcd.print(" ");
          lcd.setCursor(6, 2);
          lcd.print(consKi, 1);
        } else {
          lcd.setCursor(5, 2);
          lcd.print(consKi, 1);
        }

        lcd.setCursor(1, 3);
        lcd.print(PIDList[frame_pid + 1]);
        lcd.setCursor(7, 3);
        lcd.print(consKd, 1);
        rotaryButton.read();
      }
      if((millis() - lastBlink) >= 250) {
        
        if(menu_pid == 2) lcd.setCursor(7, menu_pid);
        else lcd.setCursor(9, menu_pid);
        lcd.print(char(0xff));
      }
      break;
  }
}

void runMenu() {
  if(mode == 0) {
    if(previous_mode != mode) {
      previous_mode = mode;
      lcd.clear();
      onLoad = 0;
    }
    menu0_MainMenu();
  } 

  else 
  {
    if(menu == 1) {
      if(previous_menu != menu || previous_mode == 0) {
        previous_menu = menu;
        lcd.clear();
        onLoad = 0;
      }
      menu1_constCurrent();
    } 

    else if(menu == 2) {
      if(previous_menu != menu || previous_mode == 0) {
        previous_menu = menu;
        lcd.clear();
        onLoad = 0;
      }
      menu2_constPower();
    } 
    
    else if(menu == 3) {
      if(previous_menu != menu || previous_mode == 0) {
        previous_menu = menu;
        lcd.clear();
        onLoad = 0;
      }
      menu3_constResistance();
    } 

    else if(menu == 4) {
      if(previous_menu != menu || previous_mode == 0) {
        previous_menu = menu;
        lcd.clear();
        onLoad = 0;
      }
      menu4_VDM();
    } 

    else if(menu == 5) {
      if(previous_menu != menu || previous_mode == 0) {
        previous_menu = menu;
        lcd.clear();
        // load can on
      }
      menu5_setting();
    }

    else if(menu == 6) {
      if(previous_menu != menu || previous_mode == 0) {
        previous_menu = menu;
        lcd.clear();
        // load can on
      }
      menu6_PID();
    } else { }
  }
}

void readInput() {
  rotaryEncode();
  yellowButton.read();
  rotaryButton.read();
  redButton.read();
}

void setDuty_PID() {
  Setpoint = Iset_forCal;
  Input = I_sense;

  if(onLoad == 1) {
    // if((millis() - onLoad_time >= 1) && loadFlag == 1) {
    //   onLoad_time = millis();
    //   dutyCycle = dutyCycle + 200;
    //   ledcWrite(PWM_pinOut, dutyCycle);
    //   // if duty >= persent of Setpoint, that be going set to PID controller
    //   if(dutyCycle >= (dutyFactor * Iset_forCal) * 0.1) {
    //     loadFlag = 0;
    //   }
    // }
    // if(loadFlag == 0) {
      calPID.SetTunings(consKp, consKi, consKd);
      calPID.Compute();
      dutyCycle = dutyFactor * Output;
    // }

  } 
  else {
    loadFlag = 1;
    onLoad_time = 0;
    Setpoint = 0;
    Input = dutyCycle * dutyFactor;
    calPID.SetTunings(consKp, consKi, consKd);
    calPID.Compute();
    dutyCycle = dutyFactor * Output;
  }

  ledcWrite(PWM_pinOut, dutyCycle);

}

uint32_t updateFan_time;
void updateFanTrig(int time) {

  if((millis() - updateFan_time >= time) && AutoFanMode == true) {
    updateFan_time = millis();
    readTemp();
    if(CurrentTemp >= FanTrig_TempUpper) FanTrig = true;
    else if(CurrentTemp <= FanTrig_TempLower) FanTrig = false;
    
    if(CurrentTemp >= Temp_Max) onLoad = false;
    digitalWrite(fanCtrl_pinOut, FanTrig);
  }

  if(AutoFanMode == false ) digitalWrite(fanCtrl_pinOut, onLoad);

}

void clock_count() {
  if(millis() - lastTime >= 1000 && onLoad == 1) {
    lastTime = millis();
    if(sec != 60) {
      sec++;
    }
    else{
      sec = 0;
      minut++;
    }
    if(minut == 100) {
      minut = 0;
    }
    if(minut >= timeMax) {
      onLoad = 0;
    }
  }
}

void setup() {
  Serial.begin(112500);
  Wire.begin();
  ADS.begin();

  lcd.init();
  lcd.backlight();
  lcd.createChar(0, arrow);
  lcd.createChar(1, subBar1);
  lcd.createChar(2, subBar2);
  lcd.createChar(3, subBar3);
  lcd.createChar(4, subBar4);
  lcd.createChar(5, subBar5);
  printBegin();
  analogReadResolution(4096);
  // analogWriteResolution(buzzer_pin ,12);
  // analogWriteFrequency(buzzer_pin, 5000);

  // AnalogToDigital pin didn't make tham are INPUT Mode!
  // pinMode(I_sense_pin, INPUT);
  // pinMode(V_sense_1st_pin, INPUT);
  // pinMode(V_sense_2nd_pin, INPUT);
  // pinMode(temp_sense_pin, INPUT);
  pinMode(buzzer_pin, OUTPUT);
  // pinMode(red_button_pin, INPUT_PULLUP);
  // pinMode(yellow_button_pin, INPUT_PULLUP);
  // pinMode(rotary_button_pin, INPUT_PULLUP);
  pinMode(Encoder_CLK, INPUT);
  pinMode(Encoder_DT, INPUT);

  pinMode(PWM_pinOut, OUTPUT);
  pinMode(fanCtrl_pinOut, OUTPUT);
  pinMode(gateCtrl_pinOut, OUTPUT);

  digitalWrite(PWM_pinOut, false);
  digitalWrite(fanCtrl_pinOut, false);
  digitalWrite(gateCtrl_pinOut, false);

  // Configed Red Button
  redButton.onPress(pressRed)
  .onDoublePress(pressRed) // default timeout
  .onPressFor(pressRed, 1000); // custom timeout for 1 second

  // Configed Yellow Button
  yellowButton.onPress(pressYellow)
  .onDoublePress(pressYellow) // default timeout
  .onPressFor(pressYellow, 1000); // custom timeout for 1 second

  // Configed Rotary Button
  rotaryButton.onPress(pressRotary)
  .onDoublePress(pressRotary) // default timeout
  .onPressFor(pressRotary, 1000); // custom timeout for 1 second

  // Pulse Width Modulation Configed
  ledcAttach(PWM_pinOut, frequency, resolution);
  dutyFactor = dutyCycle_Max / IMax;

  //PID Tuner
  calPID.SetOutputLimits(Imin, Iin_maximum);
  calPID.SetSampleTime(1);
  calPID.SetMode(AUTOMATIC);

}

void loop() {
  readInput();
  runMenu();
  rotaryEncode();
  readSensor();
  
  // Set Duty cycle
  
  setDuty_PID();

  Serial.print("duty:");
  Serial.print(dutyCycle*0.00152);
  Serial.print(",");

  Serial.print("I:");
  Serial.println(I_sense);


  // update sensor every 0.5 secconds
  updateSensor(); 
  if(menu == 1 || menu == 2 || menu == 3) {
    updateRunMode();
  }

  updateFanTrig(3000);
  
  rotaryEncode();

  clock_count();
}
