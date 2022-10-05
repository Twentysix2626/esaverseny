#include <Wire.h>
#include <LiquidCrystal.h>
#include <BH1750.h>
BH1750 lightMeter;

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

void setup(){
  Serial.begin(9600);
  Wire.begin();
  lightMeter.begin();
  Serial.println(F("BH1750 Test"));
  lcd.begin(16, 2);
  lcd.print("Ouranos Project 2022");
  delay(1500);
}

void loop() {
  lcd.clear();
  unsigned long int lux = lightMeter.readLightLevel()*100;
  Serial.print("Light: ");
  Serial.print(lux);
  Serial.println(" lx");
  lcd.setCursor(0, 0);
  lcd.print(lux / 100 );
  lcd.print(".");
  lcd.print(lux % 100);
  delay(1000);
}
