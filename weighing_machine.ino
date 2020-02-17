/*************************************
* This code is written by Andrii Valchuk
* GitHub: https://github.com/AndriiVal
* 
* 
* Microcontroller: ATmega328P (Old)
* Dev board:       Arduino nano
* LCD 1602
* HX711
*************************************/

#include "HX711.h"
#include <LiquidCrystal.h>
#include <EEPROM.h>

/*
 The circuit:
 --- LCD ---
 * LCD RS pin to digital pin 12
 * LCD Enable pin to digital pin 11
 * LCD D4 pin to digital pin 5
 * LCD D5 pin to digital pin 4
 * LCD D6 pin to digital pin 3
 * LCD D7 pin to digital pin 2
 * LCD R/W pin to ground
 * LCD VSS pin to ground
 * LCD VCC(VDD) pin to 5V
 * LCD A pin to 5V
 * LCD K pin to ground
 * 10K resistor:
   ends to +5V and ground
   wiper to LCD VO pin (pin 3)
 --- HX711 ---
 *DOUT (DT) pin to digital pin 7
 *CLK (SCK) pin to digital pin 6
 --- Buzzer to digital pin 10 and ground
 --- Button1 zeroing to digital pin 8 and ground
 --- Button2 calibration to digital pin 9 and ground
 --- Load cell connections with HX711 module ---
 * RED Wire is connected to E+
 * BLACK Wire is connected to E-
 * WHITE Wire is connected to A-
 * GREEN/BLUE Wire is connected to A+ 
*/

const int DOUT = 7; //DT
const int CLK = 6; //SCK
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
const int buzzer = 10;
boolean butt_flag1 = 0;
boolean butt1;
boolean butt_flag2 = 0;
boolean butt2;
unsigned long last_press;
int mario[] = {261, 523, 220, 440, 233, 466};
HX711 scale;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
float myScale = 0;
float oldScale = 0;
int address = 0;
float myRatio;

void zeroing() {
  lcd.clear();
  lcd.print("Reset Zero");
  lcd.setCursor(0,1);
  lcd.print("Please Wait...");
  for (int n = 0; n < 6; n++){
    tone(buzzer, mario[n]);
    delay(300);
  }
  noTone(buzzer);
  scale.power_up(); 
  scale.tare();
  scale.power_down();
  lcd.clear();
  lcd.print("Weight");
  lcd.setCursor(0,1);
  lcd.print("0.00 g");
  tone(buzzer, 988);
  delay(150);
  tone(buzzer, 1318);
  delay(1050);
  noTone(buzzer);
}

void calibration(){
  lcd.clear();
  lcd.print("Calibration...");
  lcd.setCursor(0,1);
  lcd.print("Please Wait...");
  for (int n = 0; n < 6; n++){
    tone(buzzer, mario[n]);
    delay(300);
  }
  noTone(buzzer);
  scale.power_up();
  scale.set_scale();
  scale.tare();
  tone(buzzer, 466);
  delay(150);
  tone(buzzer, 523);
  delay(150);
  tone(buzzer, 988);
  delay(150);
  noTone(buzzer);
  lcd.clear();
  lcd.print("Ready");
  lcd.setCursor(0,1);
  lcd.print("Put 100g & wait");
  delay(5000);
  myScale = scale.get_units(10);
  myRatio = myScale/100.0;         // can put here your calibrating weight / тут можна замініити калібруючу вагу на іншу в грамах
  scale.set_scale(myRatio);
  scale.power_down();
  EEPROM.put(address, myRatio);
  lcd.clear();
  lcd.print("Set scale: ");
  lcd.setCursor(0, 1);
  lcd.print(myRatio);
  tone(buzzer, 988);
  delay(150);
  tone(buzzer, 1318);
  delay(1050);
  noTone(buzzer);
}

void setup(){
  pinMode(8, INPUT_PULLUP);
  pinMode(9, INPUT_PULLUP);
  lcd.begin(16,2);
  scale.begin(DOUT, CLK);
  scale.set_scale(EEPROM.get(address, myRatio));
  zeroing();
}

void loop() {
  scale.power_up();
  myScale = scale.get_units(10);
  lcd.clear();
  lcd.print("Weight");
  lcd.setCursor(0,1);
  if (myScale > 0.5){
    if (abs(myScale - oldScale) > 0.5){
      lcd.print(myScale);
      lcd.print(" g            ");
      oldScale = myScale;
    }
    else
      lcd.print(oldScale);
      lcd.print(" g            ");
  }
  else
    lcd.print("0.00 g            ");
  scale.power_down();
  
  butt1 = !digitalRead(8);
  if (butt1 == 1 && butt_flag1 == 0 && millis() - last_press > 100) {
    butt_flag1 = 1;
    last_press = millis();
    zeroing();
  }
  if (butt1 == 0 && butt_flag1 == 1) {
    butt_flag1 = 0;
  }
  
  butt2 = !digitalRead(9);
  if (butt2 == 1 && butt_flag2 == 0 && millis() - last_press > 100) {
    butt_flag2 = 1;
    last_press = millis();
    calibration();
  }
  if (butt2 == 0 && butt_flag2 == 1) {
    butt_flag2 = 0;
  }
}
