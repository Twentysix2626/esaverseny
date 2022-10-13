#include <Wire.h>
#include <LiquidCrystal.h> // LCD Kijelzo
#include <BH1750.h> // Fenyszenzor

// Szonar
#define echoPin 7
#define trigPin 6

// Fenyszenzor
BH1750 lightMeter;

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// Szonar variables
long duration;
int distance;

void setup(){
  Serial.begin(9600);
  Wire.begin();
  lightMeter.begin();
  lcd.begin(16, 2);
  // Szonar
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  lcd.print("Ouranos Project 2022");
  delay(1500);
}

void loop() {
  lcd.clear();
  // ----- Szamitasok:

  // Fenyszenzor
  unsigned long int lux = lightMeter.readLightLevel()*100;

  // Szonar
  // trigPin torlese
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // trigPin bekapcsol 10 microszekundumig
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Szonar szamitas
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;
  

  // Kiiras
  // File:
  char buffer[5];
  Serial.print("#S|LIGHTSENS|["); 
  Serial.print(itoa((lux), buffer, 10)); 
  Serial.println("]#");
  // Serial
  Serial.print("Light: ");
  Serial.print(lux);
  Serial.println(" lx");
  Serial.print("Distance: ");
  if (distance < 451) {
    Serial.print(distance);
  }
  Serial.println(" cm");
  // LCD
  lcd.setCursor(0, 0);
  lcd.print(lux / 100);
  lcd.print(".");
  lcd.print(lux % 100);
  lcd.setCursor(0, 1);
  if (distance > 450) {
    lcd.print("INVALID");
  }else {
    lcd.print(distance);
    lcd.print("cm");
  }
 
  delay(200);
}
