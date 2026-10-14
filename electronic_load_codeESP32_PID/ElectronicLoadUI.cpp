#include "ElectronicLoadUI.h"
#include "ElectronicLoadConfig.h"
#include "ElectronicLoadDisplay.h"
#include "ElectronicLoadCapacityTest.h"
#include "BfButton.h"

#include <math.h>
#include <stdio.h>

extern float CurrentTemp;
extern int menu;
extern uint8_t mode;
extern uint32_t lastDisplayTempSensor;
extern uint32_t LastupdateSensor;
extern double V_sense_select;
extern double I_sense;
extern double PowerOut;
extern double Resistance;
extern bool onLoad;
extern uint32_t lastPressRedButton;
extern uint8_t minut;
extern uint8_t sec;
extern uint32_t lastBlinkArrow;
extern byte arrow[8];
extern uint8_t frame;
extern const char *menuList[7];
extern uint8_t previous_mode;
extern double Iset;
extern double Pset;
extern double Rset;
extern double V_sense_1st;
extern double V_sense_2nd;
extern int lastNumSubBars1;
extern int lastNumSubBars2;
extern int menu_setting;
extern int frame_setting;
extern const char *settingList[6];
extern double IMax;
extern double Iin_maximum;
extern int cursor;
extern const char *FanList[2];
extern int timeMax;
extern int Temp_Max;
extern int menu_pid;
extern int frame_pid;
extern const char *PIDList[3];
extern double consKp;
extern double consKi;
extern double consKd;
extern uint8_t previous_menu;
extern uint32_t lastBlink;
extern uint8_t select_digit;
extern BfButton rotaryButton;
extern ElectronicLoadCapacityTest capacityTest;
extern double capacityCurrentA;
extern double capacityCutoffV;
extern uint8_t capacityEditField;
extern bool temperatureValid;
extern int lastTempRaw;
extern bool capacityStartReady;

void readVoltageAll();
void rotaryEncode();

namespace {
const char ohmUnit[] = { static_cast<char>(244), '\0' };

void writeCapacityRow(uint8_t row, const char *text) {
  char line[21];
  uint8_t column = 0;
  while (column < 20 && text[column] != '\0') {
    line[column] = text[column];
    column++;
  }
  while (column < 20) line[column++] = ' ';
  line[20] = '\0';
  lcd.setCursor(0, row);
  lcd.print(line);
}
}

void updateTempSensor() {
  if(!temperatureValid) {
    char status[6];
    snprintf(status, sizeof(status), "E%04d", lastTempRaw);
    lcd.setCursor(15, 0);
    lcd.print(status);
    return;
  }

  // Temperature is sampled by the main scheduler; this function only paints it.
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

void updateSensor() {
  if( (menu == 1 ||  menu == 2 || menu == 3) && mode != 0 ) {
    if(millis() - lastDisplayTempSensor >= updateTemp_time) {
      lastDisplayTempSensor = millis();
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

void drawSetValue_setMode(float value, const char *txt, uint8_t digit) {
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

void drawSetValue_RunMode(float value, const char *txt) {
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

      if(millis() - lastDisplayTempSensor >= updateTemp_time) {
        lastDisplayTempSensor = millis();
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
        drawSetValue_RunMode(Rset, ohmUnit);
      }

      if(millis() - lastDisplayTempSensor >= updateTemp_time) {
        lastDisplayTempSensor = millis();
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
        drawSetValue_RunMode(Rset, ohmUnit);
      }

      if((millis() - lastBlink) >= 400) {
        lastBlink = millis();
        rotaryEncode();
        drawSetValue_setMode(Rset, ohmUnit, 4);
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
      //   drawSetValue_setMode(Rset, ohmUnit, 4);
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

void menu7_capacityTest() {
  static uint8_t lastScreenMode = 255;
  if (lastScreenMode != mode) {
    lastScreenMode = mode;
    lcd.clear();
  }

  char line[21];
  if (mode == 2) {
    writeCapacityRow(0, "\xff" "Battery Test Setup");
    snprintf(line, sizeof(line), "%cCurrent: %5.1fA",
             capacityEditField == 0 ? static_cast<char>(0xff) : ' ', capacityCurrentA);
    writeCapacityRow(1, line);
    snprintf(line, sizeof(line), "%cCutoff:  %5.1fV",
             capacityEditField == 1 ? static_cast<char>(0xff) : ' ', capacityCutoffV);
    writeCapacityRow(2, line);
    writeCapacityRow(3, "Dial +/-  R:Field");
    return;
  }

  writeCapacityRow(0, "\xff" "Battery Capacity");
  snprintf(line, sizeof(line), "I %5.1fA C %5.1fV", capacityCurrentA, capacityCutoffV);
  writeCapacityRow(1, line);

  switch (capacityTest.state()) {
    case ElectronicLoadCapacityTest::RUNNING: {
      const uint32_t elapsed = capacityTest.elapsedMs(millis()) / 1000;
      snprintf(line, sizeof(line), "RUN %02lu:%02lu %5.1fmAh",
               static_cast<unsigned long>(elapsed / 60),
               static_cast<unsigned long>(elapsed % 60),
               capacityTest.milliampHours());
      writeCapacityRow(2, line);
      writeCapacityRow(3, "R:Stop  Y:Back");
      break;
    }
    case ElectronicLoadCapacityTest::COMPLETE:
      snprintf(line, sizeof(line), "DONE %7.1fmAh", capacityTest.milliampHours());
      writeCapacityRow(2, line);
      snprintf(line, sizeof(line), "Wh: %6.2f  R:Reset", capacityTest.wattHours());
      writeCapacityRow(3, line);
      break;
    case ElectronicLoadCapacityTest::STOPPED:
      snprintf(line, sizeof(line), "STOP %7.1fmAh", capacityTest.milliampHours());
      writeCapacityRow(2, line);
      snprintf(line, sizeof(line), "Wh: %6.2f  R:Reset", capacityTest.wattHours());
      writeCapacityRow(3, line);
      break;
    case ElectronicLoadCapacityTest::FAULT:
      writeCapacityRow(2, "FAULT: LOAD OFF");
      writeCapacityRow(3, "R:Reset  Y:Back");
      break;
    case ElectronicLoadCapacityTest::IDLE:
    default:
      if (capacityCutoffV <= 0.1) writeCapacityRow(2, "SET CUTOFF FIRST");
      else if (!capacityStartReady) writeCapacityRow(2, "CHECK INPUT/TEMP");
      else writeCapacityRow(2, "READY");
      writeCapacityRow(3, "R:Start  Y:Edit");
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
      menu7_capacityTest();
    }

    else if(menu == 5) {
      if(previous_menu != menu || previous_mode == 0) {
        previous_menu = menu;
        lcd.clear();
        onLoad = 0;
      }
      menu4_VDM();
    }

    else if(menu == 6) {
      if(previous_menu != menu || previous_mode == 0) {
        previous_menu = menu;
        lcd.clear();
        // load can on
      }
      menu5_setting();
    }

    else if(menu == 7) {
      if(previous_menu != menu || previous_mode == 0) {
        previous_menu = menu;
        lcd.clear();
        // load can on
      }
      menu6_PID();
    } else { }
  }
}
