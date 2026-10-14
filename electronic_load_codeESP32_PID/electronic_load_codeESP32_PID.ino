#include <math.h>

#include "ElectronicLoadConfig.h"
#include "ElectronicLoadUI.h"

#include "ADS1X15.h"
#include "BfButton.h"
#include "PID_v1.h"
#include "ElectronicLoadDisplay.h"
#include "ElectronicLoadSensors.h"
#include "ElectronicLoadCapacityTest.h"
// ADC INPUT
// Move to 16Bits ADC I2C bus at: 0x48


// Digital INPUT

// Digital OUTPUT

// Setting time


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
bool temperatureValid = false;
uint32_t temperatureUpdatedAt = 0;
bool currentSampleFresh = false;
bool voltageSampleFresh = false;
uint32_t lastSerialTelemetry = 0;
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
double Rmin = 0.2;
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
int menuItemMax = 7;
int menuItemmin = 1;
int settingItemMax = 6;


//Define Variables we'll be connecting to
double Setpoint, Input, Output;

//Define the aggressive and conservative Tuning Parameters
// Don't touch Kd, it will be osscilate
double consKp=0.0, consKi=40.0, consKd=0.0;

double gainMax = 800.00;
double gainmin = 0.00;

PID calPID(&Input, &Output, &Setpoint, consKp, consKi, consKd, DIRECT);

const char *menuList[7] = {
  "Const Current     ",
  "Const Power       ",
  "Const Resistance  ",
  "Battery Capacity  ",
  "Digital Voltmeter ",
  "Utility           ",
  "PID Tuner         ",
};

int menu_setting = 1;
int frame_setting = 1;
int cursor = 1;
int cursorMax = 2;
int cursormin = 1;


const char *settingList[6]= {
  "Current Max       ",
  "Volt sense        ",
  "Fan Trig          ",
  "Runtime Max       ",
  "Shutdown Temp     ",
  "                  ",
};

const char *onOffList[2] = {
  "ON                ",
  "OFF               "
};

const char *FanList[2] = {
  "Auto              ",
  "Manual            "
};


const char *PIDList[3]= {
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
ElectronicLoadSensors sensorSampler(ADS);
ElectronicLoadCapacityTest capacityTest;
double capacityCurrentA = 1.00;
double capacityCutoffV = 0.00;
uint8_t capacityEditField = 0;
bool capacityStartReady = false;
int16_t I_ADC ;
int16_t V0_ADC ;
int16_t V1_ADC ;



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

void tryStartCapacityTest();
void serviceCapacityTest(uint32_t now, bool newCurrentSample, bool newVoltageSample);
bool sensorsReady(uint32_t now);
void serviceSensorSampler(uint32_t now);

void pressRed (BfButton *redButton, BfButton::press_pattern_t pattern) {
  switch (pattern) {
    case BfButton::SINGLE_PRESS:
      if(menu == 4 && mode == 1) {
        if(capacityTest.running()) {
          capacityTest.stop(millis());
          onLoad = false;
        } else if(capacityTest.state() == ElectronicLoadCapacityTest::COMPLETE ||
                  capacityTest.state() == ElectronicLoadCapacityTest::STOPPED ||
                  capacityTest.state() == ElectronicLoadCapacityTest::FAULT) {
          capacityTest.reset(millis());
          onLoad = false;
        } else {
          tryStartCapacityTest();
        }
        break;
      }
      if(mode == 1) {
        onLoad = !onLoad;
      }
      break;
    case BfButton::DOUBLE_PRESS:
      break;
    case BfButton::LONG_PRESS:
      if(mode == 1 && menu != 4) {
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
      if(menu == 4 && capacityTest.running()) {
        capacityTest.stop(millis());
        onLoad = false;
      }
      if(mode != 0) mode = mode - 1;
      if(mode == 0) lcd.clear();
      
      break;
    case BfButton::DOUBLE_PRESS:
      
      break;
    case BfButton::LONG_PRESS:
      if(mode == 0) {
        if(menu == 4 && capacityTest.running()) {
          capacityTest.stop(millis());
          onLoad = false;
        }
        mode = 1;
      }
      else if(mode == 1) {
        if(menu == 4 && capacityTest.running()) {
          capacityTest.stop(millis());
          onLoad = false;
        }
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
        case 4:
          capacityEditField = (capacityEditField + 1) % 2;
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
  lcd.flush();
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

    else if(menu == 6) {
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

    else if(menu == 7) {
      if(menu_pid == 1) count_trig_float(&consKp, &digit_2, &gainMax, &gainmin, state);
      else if(menu_pid == 2) count_trig_float(&consKi, &digit_3, &gainMax, &gainmin, state);
      else if(menu_pid == 3) count_trig_float(&consKd, &digit_2, &gainMax, &gainmin, state);
      
    }

    else if(menu == 4 && !capacityTest.running()) {
      if(capacityEditField == 0) {
        capacityCurrentA += state ? 0.1 : -0.1;
        if(capacityCurrentA < 0.1) capacityCurrentA = 0.1;
        if(capacityCurrentA > Iin_maximum) capacityCurrentA = Iin_maximum;
      } else {
        capacityCutoffV += state ? 0.1 : -0.1;
        if(capacityCutoffV < 0.0) capacityCutoffV = 0.0;
        if(capacityCutoffV > 999.9) capacityCutoffV = 999.9;
      }
    }

  } else 

  if(mode == 1) {
    if(menu == 6) count_trig_int(&menu_setting, &digit_1, &settingItemMax, &menuItemmin, !state);
    else if(menu == 7) count_trig_int(&menu_pid, &digit_1, &cursorPIDMax, &cursormin, !state);
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
  const int raw = analogRead(temp_sense_pin);
  if(raw <= 0 || raw >= 4095) {
    temperatureValid = false;
    return;
  }
  const float v_ntc = (raw * 3.3f) / 4095.0f;
  const float R_ntc = ((3.3f * 11000.0f) / v_ntc) - 11000.0f;
  if(!isfinite(R_ntc) || R_ntc <= 0.0f) {
    temperatureValid = false;
    return;
  }
  const float measuredTemp = (beta / (log(R_ntc / R25) + (beta / To))) - 273.0f + 4.0f + adj_temp;
  if(!isfinite(measuredTemp) || measuredTemp < -40.0f || measuredTemp > 200.0f) {
    temperatureValid = false;
    return;
  }
  CurrentTemp = measuredTemp;
  temperatureUpdatedAt = millis();
  temperatureValid = true;
  if(CurrentTemp >= Temp_Max) {
    onLoad = false;
    dutyCycle = 0;
    Output = 0.0;
    ledcWrite(PWM_pinOut, 0);
    digitalWrite(gateCtrl_pinOut, LOW);
  }
}


void readVoltageAll() {
  // Values are maintained by the non-blocking sensor scheduler.
}

bool sensorsReady(uint32_t now) {
  const uint8_t voltageIndex = V_sense_number ? 1 : 0;
  return sensorSampler.healthy() && sensorSampler.currentValid() &&
         sensorSampler.voltageValid(voltageIndex) &&
         now - sensorSampler.currentTimestamp() <= SENSOR_STALE_TIMEOUT_MS &&
         now - sensorSampler.voltageTimestamp(voltageIndex) <= SENSOR_STALE_TIMEOUT_MS &&
         temperatureValid && now - temperatureUpdatedAt <= TEMPERATURE_STALE_TIMEOUT_MS;
}

void serviceSensorSampler(uint32_t now) {
  currentSampleFresh = false;
  voltageSampleFresh = false;
  ElectronicLoadSensors::Sample sample;
  if(!sensorSampler.service(now, sample)) return;
  if(sample.channel == 0) {
    I_ADC = sample.raw;
    I_sense = fabs((double)I_ADC * 0.001250);
    currentSampleFresh = true;
  } else if(sample.channel == 1) {
    V0_ADC = sample.raw;
    V_sense_1st = fabs((double)V0_ADC * 0.001250);
    voltageSampleFresh = V_sense_number == 0;
  } else {
    V1_ADC = sample.raw;
    V_sense_2nd = fabs((double)V1_ADC * 0.001250);
    voltageSampleFresh = V_sense_number == 1;
  }

  V_sense_select = V_sense_number ? V_sense_2nd : V_sense_1st;
  PowerOut = I_sense * V_sense_select;
  Resistance = I_sense > 0.0001 ? V_sense_select / I_sense : 1000000.0;

  if(V_sense_select < VIN_MIN) {
    Iset_forCal = 0.0;
    onLoad = false;
    dutyCycle = 0;
    Output = 0.0;
    ledcWrite(PWM_pinOut, 0);
    digitalWrite(gateCtrl_pinOut, LOW);
  } else if(menu == 1) {
    Iset_forCal = Iset;
  } else if(menu == 2) {
    Iset_forCal = Pset / V_sense_select;
  } else if(menu == 3 && Rset > 0.01) {
    Iset_forCal = V_sense_select / Rset;
  } else if(menu == 4) {
    Iset_forCal = capacityCurrentA;
  }
  if(Iset_forCal > Iin_maximum) Iset_forCal = Iin_maximum;
}

void tryStartCapacityTest() {
  const uint32_t now = millis();
  capacityStartReady = sensorsReady(now) && V_sense_select >= VIN_MIN &&
                       capacityCutoffV > 0.1 && V_sense_select > capacityCutoffV;
  if(!capacityStartReady) return;
  if(capacityTest.start(now, capacityCurrentA, capacityCutoffV, Iin_maximum,
                        V_sense_select, I_sense, CurrentTemp, sensorsReady(now),
                        temperatureValid, Temp_Max)) {
    Iset_forCal = capacityCurrentA;
    onLoad = true;
  }
}

void serviceCapacityTest(uint32_t now, bool newCurrentSample, bool newVoltageSample) {
  capacityStartReady = sensorsReady(now) && V_sense_select >= VIN_MIN &&
                       capacityCutoffV > 0.1 && V_sense_select > capacityCutoffV;
  capacityTest.update(now, newCurrentSample, newVoltageSample, V_sense_select,
                      I_sense, CurrentTemp, sensorsReady(now), temperatureValid,
                      Temp_Max, (uint32_t)timeMax * 60000UL);
  if(!capacityTest.running()) {
    onLoad = false;
    dutyCycle = 0;
    Output = 0.0;
    ledcWrite(PWM_pinOut, 0);
    digitalWrite(gateCtrl_pinOut, LOW);
  }
}

void sendSerialTelemetry(uint32_t now) {
  if(now - lastSerialTelemetry < SERIAL_TELEMETRY_INTERVAL_MS) return;
  lastSerialTelemetry = now;
  // Serial Oscilloscope maps numeric CSV fields in order: CH1=voltage, CH2=current.
  Serial.print(V_sense_select, 3);
  Serial.write(',');
  Serial.println(I_sense, 3);
}



int lastNumSubBars1;
int lastNumSubBars2;












void readInput() {
  rotaryEncode();
  yellowButton.read();
  rotaryButton.read();
  redButton.read();
}

void setDuty_PID() {
  static bool wasEnabled = false;
  static double lastKp = NAN, lastKi = NAN, lastKd = NAN;
  if(!sensorsReady(millis()) || !onLoad || Iset_forCal <= 0.0) {
    onLoad = false;
    wasEnabled = false;
    dutyCycle = 0;
    Output = 0.0;
    ledcWrite(PWM_pinOut, 0);
    digitalWrite(gateCtrl_pinOut, LOW);
    return;
  }
  Setpoint = Iset_forCal;
  Input = I_sense;
  if(consKp != lastKp || consKi != lastKi || consKd != lastKd) {
    calPID.SetTunings(consKp, consKi, consKd);
    lastKp = consKp; lastKi = consKi; lastKd = consKd;
  }
  if(!wasEnabled) {
    Output = 0.0;
    calPID.SetMode(MANUAL);
    calPID.SetMode(AUTOMATIC);
    wasEnabled = true;
  }
  if(calPID.Compute()) {
    dutyCycle = (int)(dutyFactor * Output);
    ledcWrite(PWM_pinOut, dutyCycle);
  }
  digitalWrite(gateCtrl_pinOut, HIGH);
}

uint32_t updateFan_time;
void updateFanTrig(int time) {

  if((millis() - updateFan_time >= time) && AutoFanMode == true) {
    updateFan_time = millis();
    if(temperatureValid) {
      if(CurrentTemp >= FanTrig_TempUpper) FanTrig = true;
      else if(CurrentTemp <= FanTrig_TempLower) FanTrig = false;
    }
  }
  if(AutoFanMode) digitalWrite(fanCtrl_pinOut, FanTrig || !temperatureValid);
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
  Serial.begin(115200);
  Wire.begin();

  lcd.init();
  lcd.backlight();
  lcd.createChar(0, arrow);
  lcd.createChar(1, subBar1);
  lcd.createChar(2, subBar2);
  lcd.createChar(3, subBar3);
  lcd.createChar(4, subBar4);
  lcd.createChar(5, subBar5);
  printBegin();
  analogReadResolution(12);
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
  calPID.SetSampleTime(PID_SAMPLE_INTERVAL_MS);
  calPID.SetMode(AUTOMATIC);
  sensorSampler.begin(millis());
  readTemp();

}

void loop() {
  const uint32_t now = millis();
  readInput();
  runMenu();
  serviceSensorSampler(now);
  if(now - lastUpdateTempSensor >= updateTemp_time) {
    lastUpdateTempSensor = now;
    readTemp();
  }
  if(capacityTest.running() && (menu != 4 || mode != 1)) {
    capacityTest.stop(now);
    onLoad = false;
  }
  if(menu == 4 && capacityTest.running()) Iset_forCal = capacityCurrentA;
  if(menu == 4) serviceCapacityTest(now, currentSampleFresh, voltageSampleFresh);
  setDuty_PID();

  updateSensor(); 
  if(menu == 1 || menu == 2 || menu == 3) {
    updateRunMode();
  }

  updateFanTrig(3000);
  if(menu != 4) clock_count();
  sendSerialTelemetry(now);
  lcd.flush();
}
