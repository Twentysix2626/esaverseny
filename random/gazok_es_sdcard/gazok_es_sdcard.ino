#include <SPI.h>
#include <SD.h>
#include <MQUnifiedsensor.h>

#define alaplap "Arduino UNO"
#define Voltage_Resolution 5
#define pin1 A0
#define pin2 A1
#define type1 "MQ-135"
#define type2 "MQ-7"
#define ADC_Bit_Resolution 10
#define RatioMQ135CleanAir 3.6
#define RatioMQ7CleanAir 27.5


MQUnifiedsensor MQ135(alaplap, Voltage_Resolution, ADC_Bit_Resolution, pin1, type1);
MQUnifiedsensor MQ7(alaplap, Voltage_Resolution, ADC_Bit_Resolution, pin2, type2);
File adatFile;


void setup() {
  Serial.begin(9600);
  Serial.println("Na helo, most az van hogy tesztelni fogjuk minden megy-e mert ha nem megy akkor nagy bajban leszel egy het utan");

  Serial.println("SD Kartya tesztelese:");

  if (!SD.begin(4)) {
    Serial.println("Valami baj van haver az SDKartyaval amugy, old meg");
    while (1);
  }
  Serial.println("Jol van a kartya oreg");

  adatFile = SD.open("adat.txt", FILE_WRITE);

  if (adatFile) {
    Serial.println("Teszteljuk lehet-e irni a kartyara:");
    adatFile.println("Na lehet irni ugy latom:)");
    Serial.println("Ugy latom lehet is irni nagyon pompas");
  }else {
    Serial.println("Nem tudtam megnyitani az adat.txt-t, van valami otlteted?");
  }

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

  Serial.println("MQ-7 kalibracio");
  MQ7.setRegressionMethod(1);
  MQ7.init(); 
  calcR0 = 0;
  for(int i = 1; i<=10; i ++)
  {
    MQ7.update();
    calcR0 += MQ7.calibrate(RatioMQ7CleanAir);
  }
  MQ7.setR0(calcR0/10);

  if (isinf(calcR0)) {
    Serial.println("Csatlakozasi hiba: R0 vegtelen; KABELEK"); 
    while(1);
  }
  if (calcR0 == 0) {
    Serial.println("Csatlakozasi hiba, R0 = 0; ANALOG PIN KABELEK");
    while(1);
  }


  adatFile.println("MQ-135  |  MQ-7");
  adatFile.println("MS  |  CO  Alcohol  CO2  Toluen  NH4  Aceton  |  H2  LPG  CH4  CO  Alcohol");  

  Serial.println("Ugy latom haver minden rendben, jegyezd le a datumot es idot valahova, ez fontos!");
  adatFile.close();
  delay(5000);
}

void loop() {
  adatFile = SD.open("adat.txt", FILE_WRITE);
  adatFile.print(millis());
  adatFile.print("  ");
  MQ135.update();
  MQ7.update();

  MQ135.setA(605.18); MQ135.setB(-3.937);
  float CO = MQ135.readSensor();
  MQ135.setA(77.255); MQ135.setB(-3.18);
  float Alcohol = MQ135.readSensor();
  MQ135.setA(110.47); MQ135.setB(-2.862);
  float CO2 = MQ135.readSensor();
  MQ135.setA(44.947); MQ135.setB(-3.445);
  float Toluen = MQ135.readSensor();
  MQ135.setA(102.2 ); MQ135.setB(-2.473);
  float NH4 = MQ135.readSensor();
  MQ135.setA(34.668); MQ135.setB(-3.369);
  float Aceton = MQ135.readSensor();
  MQ7.setA(69.014); MQ7.setB(-1.374);
  float H2 = MQ7.readSensor();
  MQ7.setA(700000000); MQ7.setB(-7.703);
  float LPG = MQ7.readSensor();
  MQ7.setA(60000000000000); MQ7.setB(-10.54);
  float CH4 = MQ7.readSensor();
  MQ7.setA(99.042); MQ7.setB(-1.518);
  float CO_2 = MQ7.readSensor();
  MQ7.setA(40000000000000000); MQ7.setB(-12.35);
  float Alcohol_2 = MQ7.readSensor();

  adatFile.print(CO); adatFile.print("  ");
  adatFile.print(Alcohol); adatFile.print("  ");
  adatFile.print(CO2); adatFile.print("  ");
  adatFile.print(Toluen); adatFile.print("  ");
  adatFile.print(NH4); adatFile.print("  ");
  adatFile.print(Aceton); adatFile.print("  |  ");
  adatFile.print(H2); adatFile.print("  ");
  adatFile.print(LPG); adatFile.print("  ");
  adatFile.print(CH4); adatFile.print("  ");
  adatFile.print(CO_2); adatFile.print("  ");
  adatFile.println(Alcohol_2);;
  Serial.println("Sikeres meres!");
  
  adatFile.close();
  delay(30000);
}
