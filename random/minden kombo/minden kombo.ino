//GY-GYPSV3 Neo
#include <TinyGPSPlus.h>
#include <SoftwareSerial.h>
//static const int RXPin = 3, TXPin = 2; // GPS TX -> Code RX stb, most mas mint az eredeti kodban 
//static const uint32_t GPSBaud = 9600;

#define RXPin 3
#define TXPin 2
#define GPSBaud 9600

TinyGPSPlus gps;

//MiCS 5524

//SD card
#include <SPI.h>
#include <SD.h>


File adatFile;

//MQ135

#include <MQUnifiedsensor.h>

#define alaplap "Arduino UNO"
#define Voltage_Resolution 5
#define pin1 A0
#define type1 "MQ-135"
#define ADC_Bit_Resolution 10
#define RatioMQ135CleanAir 3.6


MQUnifiedsensor MQ135(alaplap, Voltage_Resolution, ADC_Bit_Resolution, pin1, type1);

//BME680

//BH1750
//#include <Wire.h>
//#include <BH1750.h>

//BH1750 lightMeter;





SoftwareSerial ss(RXPin, TXPin);

//a globalis valtozok tul sok ramot hasznalnak, mit lehet tenni?
void setup()
{
  //gps
  Serial.begin(115200);
  ss.begin(GPSBaud);

  Serial.println(F("GY-GPSV3-NEO"));
  Serial.print(F("TinyGPSPlus library v. ")); Serial.println(TinyGPSPlus::libraryVersion());
  Serial.println(F("by Mikal Hart"));
  Serial.println();


  //SD setup
  if (!SD.begin(4)) {
    Serial.println(F("Valami baj van haver az SDKartyaval amugy, old meg"));
    while (1);
  }
  Serial.println(F("Jol van a kartya oreg"));

  adatFile = SD.open("adat.txt", FILE_WRITE);

  if (adatFile) {
    Serial.println("Teszteljuk lehet-e irni a kartyara:");
    adatFile.println("Na lehet irni ugy latom:)");
    Serial.println("Ugy latom lehet is irni nagyon pompas");
  }else {
    Serial.println("Nem tudtam megnyitani az adat.txt-t, van valami otlteted?");
  }

  //BH 1750 Setup
  //Wire.begin();
  //lightMeter.begin();

  //MQ-135 setup
  Serial.println("MQ-135 kalibracio");
  MQ135.setRegressionMethod(1); //_PPM =  a*ratio^b
  MQ135.init();
  float calcR0 = 0;
  for(int i = 1; i<=10; i ++)
  {
    MQ135.update();
    calcR0 += MQ135.calibrate(RatioMQ135CleanAir);
  }
  MQ135.setR0(calcR0/10);
  
  if (isinf(calcR0)) {
    Serial.println("Csatlakozasi hiba: R0 vegtelen; KABELEK"); 
    while(1);
  }
  if (calcR0 == 0) {
    Serial.println("Csatlakozasi hiba, R0 = 0; ANALOG PIN KABELEK");
    while(1);
  }
}

void loop()
{
  //SD
  adatFile = SD.open("adat.txt", FILE_WRITE);
  adatFile.print(millis());
  adatFile.print("  ");
  
  //MQ135
  MQ135.update();
  
  MQ135.setA(605.18); MQ135.setB(-3.937);
  float n = MQ135.readSensor();
  Serial.print(" O: "); Serial.print(n);
  adatFile.print("CO: "); adatFile.print(n);

  MQ135.setA(77.255); MQ135.setB(-3.18);
  n = MQ135.readSensor();
  Serial.print(" Alkohol: "); Serial.print(n);
  adatFile.print("alkohol: "); adatFile.print(n);

  MQ135.setA(110.47); MQ135.setB(-2.862);
  n = MQ135.readSensor();
  Serial.print(" CO2: "); Serial.print(n);
  adatFile.print("CO2: "); adatFile.print(n);

  MQ135.setA(44.947); MQ135.setB(-3.445);
  n = MQ135.readSensor();
  Serial.print(" Toluen: "); Serial.print(n);
  adatFile.print(" Toluen: "); adatFile.print(n);

  MQ135.setA(102.2 ); MQ135.setB(-2.473);
  n = MQ135.readSensor();
  Serial.print(" NH4: "); Serial.print(n);
  adatFile.print(" NH4: "); adatFile.print(n);
  
  MQ135.setA(34.668); MQ135.setB(-3.369); //ez mit csinal?
  Serial.println();

  //MQ135->SD
  // adatFile.print("CO: "); adatFile.print(CO);
  // adatFile.print(" Alkohol: "); adatFile.print(Alcohol);
  // adatFile.print(" CO2: "); adatFile.print(CO2);
  // adatFile.print(" Toluen: "); adatFile.print(Toluen);
  // adatFile.print(" NH4: "); adatFile.print(NH4);

  adatFile.println();
  
  //MiCS 5524
 // int reading = analogRead(A0);
  Serial.print("MiCS reading:");
  Serial.println(analogRead(A0));
  adatFile.print("MiCS reading:");
  adatFile.println(analogRead(A0));

  //BH1750
  //unsigned long int lux = lightMeter.readLightLevel()*100;
  //unsigned long int lux = 0;
  Serial.print("Light: ");
  //Serial.print(lightMeter.readLightLevel());
  Serial.println(" lx");

  adatFile.print("Light: ");
 // adatFile.print(lightMeter.readLightLevel());
  adatFile.println(" lx");


  //gps
  while (ss.available() > 0) //nem megy rendesen de idk
    if (gps.encode(ss.read()))
      {displayGPS();
       SDprintGPS();
      }

  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println(F("Nincs GPS (K�t�sek)."));
    //while(true);
  }

  delay(900);
}

void displayGPS()
{
  Serial.print(F("Location: ")); 
  if (gps.location.isValid())
  {
    Serial.print(gps.location.lat(), 6);
    Serial.print(F(","));
    Serial.print(gps.location.lng(), 6);
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.print(F("  Date/Time: "));
  if (gps.date.isValid())
  {
    Serial.print(gps.date.month());
    Serial.print(F("/"));
    Serial.print(gps.date.day());
    Serial.print(F("/"));
    Serial.print(gps.date.year());
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.print(F(" "));
  if (gps.time.isValid())
  {
    if (gps.time.hour() < 10) Serial.print(F("0"));
    Serial.print(gps.time.hour());
    Serial.print(F(":"));
    if (gps.time.minute() < 10) Serial.print(F("0"));
    Serial.print(gps.time.minute());
    Serial.print(F(":"));
    if (gps.time.second() < 10) Serial.print(F("0"));
    Serial.print(gps.time.second());
    Serial.print(F("."));
    if (gps.time.centisecond() < 10) Serial.print(F("0"));
    Serial.print(gps.time.centisecond());
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.println();
}
void SDprintGPS()
{
  adatFile.print(F("Location: ")); 
  if (gps.location.isValid())
  {
    adatFile.print(gps.location.lat(), 6);
    adatFile.print(F(","));
    adatFile.print(gps.location.lng(), 6);
  }
  else
  {
    adatFile.print(F("INVALID"));
  }

  adatFile.print(F("  Date/Time: "));
  if (gps.date.isValid())
  {
    adatFile.print(gps.date.month());
    adatFile.print(F("/"));
    adatFile.print(gps.date.day());
    adatFile.print(F("/"));
    adatFile.print(gps.date.year());
  }
  else
  {
    adatFile.print(F("INVALID"));
  }

  adatFile.print(F(" "));
  if (gps.time.isValid())
  {
    if (gps.time.hour() < 10) adatFile.print(F("0"));
    adatFile.print(gps.time.hour());
    adatFile.print(F(":"));
    if (gps.time.minute() < 10) adatFile.print(F("0"));
    adatFile.print(gps.time.minute());
    adatFile.print(F(":"));
    if (gps.time.second() < 10) adatFile.print(F("0"));
    adatFile.print(gps.time.second());
    adatFile.print(F("."));
    if (gps.time.centisecond() < 10) adatFile.print(F("0"));
    adatFile.print(gps.time.centisecond());
  }
  else
  {
    adatFile.print(F("INVALID"));
  }

  adatFile.println();
}