#include <CurieBLE.h>
#include "HX711.h"
#include <LiquidCrystal.h>

BLEPeripheral ble;
BLEService bleWeightService("19B10002-E8F2-537E-4F6C-D104768A1214"); 
BLEIntCharacteristic bleWeightMeasurement("19B10003-E8F2-537E-4F6C-D104768A1214", BLERead | BLENotify);
BLEUnsignedCharCharacteristic switchCharacteristic("19B10004-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

float weight = 0;
int previousWeight = 0;
long previousMillis = 0;
float prevVolts;

boolean check = false;

int buttonPin = 6;
int batteryPin = A5;

HX711 scale;

void setup() {
  // put your setup code here, to run once:
  pinMode(buttonPin, INPUT);

  ble.setLocalName("SCALE");
  ble.setAdvertisedServiceUuid(bleWeightService.uuid());
  ble.addAttribute(bleWeightService);
  ble.addAttribute(bleWeightMeasurement);
  ble.addAttribute(switchCharacteristic);
  
  ble.begin();

  scale.begin(A1, A0);
    
  scale.set_scale(211.2);
  scale.tare();

  weight = 0.0;

  lcd.begin(16, 2);
  lcd.setCursor(11, 0);
  lcd.print("grams");
}

void loop() {
  // put your main code here, to run repeatedly:
  tare();
  showWeight();
  
  BLECentral central = ble.central();
  
  if (central) {
    clearSecondLine();
    lcd.setCursor(0,1);
    lcd.print("BLE Connected");
    
    while (central.connected()) {
      tare();
      showWeight();
      long currentMillis = millis();
      if (currentMillis - previousMillis >= 200) {
        previousMillis = currentMillis;
        int measurement = (int) scale.get_units(1);
        bleWeightMeasurement.setValue(measurement);
      }
      if (switchCharacteristic.written()) {
          bleTare();
      }
    }
    clearSecondLine();
  }
}

void bleTare() {
      scale.tare();
      clearFirstLine();
      lcd.setCursor(0,0);
      lcd.print("TARE");
}

void tare() {
    if (digitalRead(buttonPin) == HIGH && !check) {
      check = true;
      scale.tare();
      clearFirstLine();
      lcd.setCursor(0,0);
      lcd.print("TARE");
  }

  if (digitalRead(buttonPin) == LOW) {
      check = false;
  }
}

void showWeight() {
    int currentWeight = (int) scale.get_units(1);
    if (previousWeight != currentWeight && !check) {
      clearFirstLine();
      lcd.setCursor(0,0);
      lcd.print(currentWeight);

      previousWeight = currentWeight;
  }
}

void clearFirstLine() {
  lcd.setCursor(0, 0);
  lcd.print("           ");
}

void clearSecondLine() {
  lcd.setCursor(0, 1);
  lcd.print("                ");
}

