#include <SPI.h>
#include <Ethernet.h>
#include "MgsModbus.h"

#include "RTClib.h"
RTC_DS1307 rtc;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

MgsModbus Mb;
int inByte = 0; // incoming serial byte

#define I2C_ADDRESS 0x3C
#include "SSD1306Ascii.h"
#include "SSD1306AsciiAvrI2c.h"
SSD1306AsciiAvrI2c oled;

#include <DHT.h>
#define DHTPIN 7
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

#include <SFE_BMP180.h>
#include <Wire.h>

SFE_BMP180 pressure;
#define ALTITUDE -0.5


// Ethernet settings (depending on MAC and Local network)
byte mac[] = {0x90, 0xA2, 0xDA, 0x0E, 0x94, 0xB5 };
IPAddress ip(192, 168, 1, 11);
IPAddress gateway(192, 168, 10, 1);
IPAddress subnet(255, 255, 255, 0);

int Nivel_1 = 30;
int Nivel_2 = 55;
int Nivel_3 = 80;
int Analog_1 = 40;
int Analog_2 = 50;
int Analog_3 = 60;
int Analog_4 = 70;
int Motor_1 = 1;
int Motor_2 = 0;
int Motor_3 = 1;
int Valvula_1 = 1;
int Valvula_2 = 0;
int Valvula_3 = 1;
int Valvula_4 = 0;
int Valvula_5 = 1;
int Valvula_6 = 0;


const int HH_1 = 14;
const int LL_1 = 15;
const int HH_2 = 16;
const int LL_2 = 17;
const int HH_3 = 18;
const int LL_3 = 19;

const int Mot_1 = 22;
const int Mot_2 = 23;
const int Mot_3 = 24;
const int Val_1 = 25;
const int Val_2 = 26;
const int Val_3 = 27;
const int Val_4 = 28;
const int Val_5 = 29;
const int Val_6 = 30;


void setup()
{

  oled.begin(&Adafruit128x64, I2C_ADDRESS);
  oled.clear();
  oled.setFont(TimesNewRoman16_bold);
  dht.begin();
  pressure.begin();


  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
  if (! rtc.isrunning()) {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    //rtc.adjust(DateTime(2020, 5, 15, 12, 55, 0));
  }


  // serial setup
  Serial.begin(9600);
  Serial.println("Serial interface started");

  // initialize the ethernet device
  Ethernet.begin(mac, ip, gateway, subnet);   // start etehrnet interface
  Serial.println("Ethernet interface started");

  // print your local IP address:
  Serial.print("My IP address: ");
  for (byte thisByte = 0; thisByte < 4; thisByte++) {
    // print the value of each byte of the IP address:
    Serial.print(Ethernet.localIP()[thisByte], DEC);
    Serial.print(".");
  }
  Serial.println();
  Serial3.begin(9600);
  // Fill MbData
  //  Mb.SetBit(0,false);
  /*Mb.MbData[0] = 0;
    Mb.MbData[1] = 0;
    Mb.MbData[2] = 0;
    Mb.MbData[3] = 0;
    Mb.MbData[4] = 0;
    Mb.MbData[5] = 0;
    Mb.MbData[6] = 0;
    Mb.MbData[7] = 0;
    Mb.MbData[8] = 0;
    Mb.MbData[9] = 0;
    Mb.MbData[10] = 0;
    Mb.MbData[11] = 0;
    Mb.MbData[12] = 0;
    Mb.MbData[13] = 0;
    Mb.MbData[14] = 0;
    Mb.MbData[15] = 0;*/

  // print MbData
  for (int i = 0; i < 12; i++) {
    Serial.print("address: "); Serial.print(i); Serial.print("Data: "); Serial.println(Mb.MbData[i]);
  }
  // print menu
  Serial.println("0 - print the first 12 words of the MbData space");
  Serial.println("1 - fill MbData with 0x0000 hex");
  Serial.println("2 - fill MbData with 0xFFFF hex");
  Serial.println("3 - fill MbData with 0x5555 hex");
  Serial.println("4 - fill MbData with 0xAAAA hex");


  pinMode(Mot_1, OUTPUT);
  pinMode(Mot_2, OUTPUT);
  pinMode(Mot_3, OUTPUT);

  pinMode(HH_1, INPUT);
  pinMode(LL_1, INPUT);

  pinMode(HH_2, INPUT);
  pinMode(LL_2, INPUT);

  pinMode(HH_3, INPUT);
  pinMode(LL_3, INPUT);


}

void loop()
{
  //Reloj.......................................................

  DateTime now = rtc.now();

  Mb.MbData[22] = now.year(), DEC;
  Mb.MbData[23] = now.month(), DEC;
  Mb.MbData[24] = now.day(), DEC;
  Mb.MbData[25] = now.hour(), DEC;
  Mb.MbData[26] = now.minute(), DEC;
  Mb.MbData[27] = now.second(), DEC;

  //Presion Humedad y temperatura..................................

  char status;
  double T, P, p0, a;
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  float f = dht.readTemperature(true);
  float hif = dht.computeHeatIndex(f, h);
  float hic = dht.computeHeatIndex(t, h, false);


  status = pressure.startTemperature();
  if (status != 0)
  {
    delay(status);
    status = pressure.getTemperature(T);
    if (status != 0)
    {
      status = pressure.startPressure(3);
      if (status != 0)
      {
        delay(status);
        status = pressure.getPressure(P, T);
        if (status != 0)
        {
          p0 = pressure.sealevel(P, ALTITUDE); // we're at 1655 meters (Boulder, CO)
          a = pressure.altitude(P, p0);
        }
      }
    }
  }

  P = 10 * P;
  h = 100 * h;
  T = 100 * T;

  Mb.MbData[28] = int(P);
  Mb.MbData[29] = int(h);
  Mb.MbData[30] = int(T);

  P = P / 10;
  h = h / 100;
  T = T / 100;

  //Pantalla_OLED.........................................................

  oled.print("PA: ");
  oled.print(P, 2);
  oled.println(" mb, ");
  oled.print("T1: ");
  oled.print(T);
  oled.println(" *C");
  oled.print("H: ");
  oled.print(h);
  oled.println(" %");
  oled.print(now.hour(), DEC);
  oled.print(":");
  oled.print(now.minute(), DEC);
  oled.print("   ");
  oled.print(now.day(), DEC);
  oled.print("/");
  oled.print(now.month(), DEC);
  oled.print("/");
  oled.print(now.year(), DEC);
  oled.print(" ");
  oled.setCursor(0, 0);

  //Entradas_Analogicas....................................................

  Mb.MbData[0] = Nivel_1;
  Mb.MbData[1] = Nivel_2;
  Mb.MbData[2] = Nivel_3;
  Mb.MbData[3] = Analog_1;
  Mb.MbData[4] = Analog_2;
  Mb.MbData[5] = Analog_3;
  Mb.MbData[6] = Analog_4;

  //Entradas_Digitales....................................................

  Mb.MbData[16] = digitalRead(HH_1);
  Mb.MbData[17] = digitalRead(LL_1);
  Mb.MbData[18] = digitalRead(HH_2);
  Mb.MbData[19] = digitalRead(LL_2);
  Mb.MbData[20] = digitalRead(HH_3);
  Mb.MbData[21] = digitalRead(LL_3);

  //Salidas_Digitales....................................................

  digitalWrite(Mot_1, Mb.MbData[7]);
  digitalWrite(Mot_2, Mb.MbData[8]);
  digitalWrite(Mot_3, Mb.MbData[9]);
  digitalWrite(Val_1, Mb.MbData[10]);
  digitalWrite(Val_2, Mb.MbData[11]);
  digitalWrite(Val_3, Mb.MbData[12]);
  digitalWrite(Val_4, Mb.MbData[13]);
  digitalWrite(Val_5, Mb.MbData[14]);
  digitalWrite(Val_6, Mb.MbData[15]);


  //Estados....................................................

  Serial3.print(Nivel_1);         //0
  Serial3.print(",");
  Serial3.print(Nivel_2);         //1
  Serial3.print(",");
  Serial3.print(Nivel_3);         //2
  Serial3.print(",");
  Serial3.print(Analog_1);        //3
  Serial3.print(",");
  Serial3.print(Analog_2);        //4
  Serial3.print(",");
  Serial3.print(Analog_3);        //5
  Serial3.print(",");
  Serial3.print("0");             //6
  Serial3.print(",");
  Serial3.print("0");             //7
  Serial3.print(",");
  Serial3.print("0");             //8
  Serial3.print(",");
  Serial3.print("0");             //9
  Serial3.print(",");
  Serial3.print("0");             //10
  Serial3.print(",");
  Serial3.print("0");             //11
  Serial3.print(",");
  Serial3.print("0");             //12
  Serial3.print(",");
  Serial3.print("0");             //13
  Serial3.print(",");
  Serial3.print("0");             //14
  Serial3.print(",");
  Serial3.print("0");             //15
  Serial3.print(",");
  Serial3.print("0");             //16
  Serial3.print(",");
  Serial3.print("0");             //17
  Serial3.print(",");
  Serial3.println("0");           //18

  //Modbus_Refrescamiento......................................
  if (Serial.available() > 0) {
    // get incoming byte:
    inByte = Serial.read();
    if (inByte == '0') {                                          // print MbData
      for (int i = 0; i < 12; i++) {
        Serial.print("address: "); Serial.print(i); Serial.print("Data: "); Serial.println(Mb.MbData[i]);
      }
    }
    if (inByte == '1') {
      for (int i = 0; i < 12; i++) {
        Mb.MbData[i] = 0x0000;
      }
    }
    if (inByte == '2') {
      for (int i = 0; i < 12; i++) {
        Mb.MbData[i] = 0xFFFF;
      }
    }
    if (inByte == '3') {
      for (int i = 0; i < 12; i++) {
        Mb.MbData[i] = 0x5555;
      }
    }
    if (inByte == '4') {
      for (int i = 0; i < 12; i++) {
        Mb.MbData[i] = 0xAAAA;
      }
    }
  }
  //Mb.MbmRun()......................................
  Mb.MbsRun();

}
