#include <SPI.h>
#include <Wire.h>
#include <SD.h>
#include <Adafruit_BMP280.h>
#include <EEPROM.h>
#include <ChainableLED.h>
#include <LiquidCrystal_I2C.h>
#include <AHT20.h>
//#include <TinyGPS++.h>
//#include <SoftwareSerial.h>

AHT20 aht;

/*
#include <OneWire.h>
#define ONE_WIRE_BUS 5  // Broche du capteur DS18B20

OneWire ds(ONE_WIRE_BUS);

float readDS18B20() {
    byte data[9], addr[8];

    if (!ds.search(addr)) { 
        ds.reset_search();
        return -1000;  // Valeur d'erreur si pas de capteur détecté
    }

    if (OneWire::crc8(addr, 7) != addr[7]) {
        return -1000;  // Erreur CRC
    }

    ds.reset();
    ds.select(addr);
    ds.write(0x44, 1);  // Démarrer la conversion de température

    delay(750);  // Attendre la fin de la conversion

    ds.reset();
    ds.select(addr);
    ds.write(0xBE);  // Lire les données

    for (byte i = 0; i < 9; i++) {
        data[i] = ds.read();
    }

    int16_t raw = (data[1] << 8) | data[0];  // Conversion des données
    return raw / 16.0;  // Température en °C
}
*/

// Broches
#define SD_CS_PIN 4
#define LIGHT_SENSOR_PIN A0
const uint8_t btnR = 2;
const uint8_t btnG = 3;

//Déclaration LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);

//TinyGPSPlus gps; //Déclaration de l'objet gps
//SoftwareSerial gpsSerial(7, 8);  // RX = D7, TX = D8

//Librairie RTC écrit avec Wire
byte bcd2Dec(byte val) {
  return ( (val/16*10) + (val%16) );
}

byte dec2Bcd(byte val) {
  return ( (val/10*16) + (val%10) );
}
#define DS1307_ADDRESS 0x68
uint8_t sec, mins, hr, dow, dom, mon;
uint16_t yr;

void readTime() {
  Wire.beginTransmission(DS1307_ADDRESS);
  Wire.write(0);
  Wire.endTransmission(false);

  Wire.requestFrom((uint8_t)DS1307_ADDRESS, (uint8_t)7, (uint8_t)false);

  sec = bcd2Dec(Wire.read() & 0x7F);
  mins = bcd2Dec(Wire.read());
  hr = bcd2Dec(Wire.read() & 0x3F);
  dow = bcd2Dec(Wire.read());
  dom = bcd2Dec(Wire.read());
  mon = bcd2Dec(Wire.read());
  yr = bcd2Dec(Wire.read()) + 2000;
}

void setTime(byte s, byte m, byte h, byte dW, byte dM, byte mo, uint16_t y) {
    Wire.beginTransmission(DS1307_ADDRESS);
    Wire.write(0);
    Wire.write(dec2Bcd(s) & 0x7F);
    Wire.write(dec2Bcd(m));
    Wire.write(dec2Bcd(h));
    Wire.write(dec2Bcd(dW));
    Wire.write(dec2Bcd(dM));
    Wire.write(dec2Bcd(mo));
    Wire.write(dec2Bcd(y - 2000));
    Wire.endTransmission();
}

void printTime() {
  readTime();
  Serial.print(yr);
  Serial.print('/');
  if (mon < 10) Serial.print('0');
  Serial.print(mon);
  Serial.print('/');
  if (dom < 10) Serial.print('0');
  Serial.print(dom);
  Serial.print(" ");
  if (hr < 10) Serial.print('0');
  Serial.print(hr);
  Serial.print(':');
  if (mins < 10) Serial.print('0');
  Serial.print(mins);
  Serial.print(':');
  if (sec < 10) Serial.print('0');
  Serial.println(sec);
}

//Structure de configuration (doit être définie en premier)
struct Conf {
    uint8_t logInt;
    uint16_t fileMax;
    uint8_t timeout;
    uint8_t lumin;
    uint16_t lumLow;
    uint16_t lumHigh;
    uint8_t tempAir;
    int8_t minTemp;
    int8_t maxTemp;
    uint8_t hygr;
    int8_t hygrMin;
    int8_t hygrMax;
    uint8_t pres;
    uint16_t presMin;
    uint16_t presMax;
} conf, defConf = {10, 4096, 30, 1, 255, 768, 1, -10, 60, 1, 0, 50, 1, 850, 1080};

//Définitions globales
volatile bool btnPressed = false;
unsigned long pressTime = 0;
uint8_t btnID = 0; // 1: Rouge, 2: Vert
enum SysMode { STD, CONF, MAINT, ECON };
SysMode curMode = STD;
SysMode prevMode = STD;
unsigned long modeStart = 0;
unsigned long lastAct = 0;

// Objets matériels
#define DS1307_ADDRESS 0x68
Adafruit_BMP280 bmp;
File logFile;
unsigned long lastLog = 0;
int fileRev = 0;
#define NUM_LEDS 1
ChainableLED leds(6, 7, NUM_LEDS);

float temperature;
float humidity;

struct SenData {
    float temperature;
    float humidity;
    float pres;
    int light;
    double lat;
    double lon;
};

// Fonctions de mode (initialisation)
void stdMode();
void confMode();
void maintMode();
void econMode();
void checkStartMode();

// Fonctions de mode (exécution continue)
void runStdMode();
void runConfMode();
void runMaintMode();
void runEconMode();

// Fonctions utilitaires (EEPROM, RTC, etc. - prototypes seulement)
void saveConf();
void loadConf();
void resetConf();
void printConf();

//void handleSerialInput(); // Pas utilisée
void setClock(const char *timeStr);
void setDate(const char *dateStr);
void setDay(const char *dayStr);
void procCmd(char* key, char* val);
void writeEEPROM(int addr, uint8_t* data, int len);
void readEEPROM(int addr, uint8_t* data, int len);
void readSens(SenData *data);
void logData();
void ledJaune();
void ledRouge();
void ledBlanche();
void ledVerte();
void ledBleue();
void ledOrange();
void errH();
void errGPS();
void carteP();
void errW();
void errC();
void dataI();

//Interruptions
void handleIntRED() {
  if (!btnPressed) {
    Serial.print("Bouton 1");
    //Serial.print(curMode);
    pressTime = millis();
    btnPressed = true;
    btnID = 1;
  }
}

void handleIntGREEN() {
  if (!btnPressed) {
    Serial.print("Bouton 2");
    //Serial.print(curMode);
    pressTime = millis();
    btnPressed = true;
    btnID = 2;
  }
}

//Setup
void setup() {
  Serial.begin(9600);

  pinMode(btnR, INPUT_PULLUP);
  pinMode(btnG, INPUT_PULLUP);
  digitalWrite(btnR, HIGH);
  digitalWrite(btnG, HIGH);
  attachInterrupt(digitalPinToInterrupt(btnR), handleIntRED, FALLING);
  attachInterrupt(digitalPinToInterrupt(btnG), handleIntGREEN, FALLING);

  delay(500);
  while (!Serial) { ; }

  //gpsSerial.begin(9600); //Initialisation du port série du GPS
  lcd.init();
  
  Wire.begin();
  if (!rtcConnected()) {
       //lcd.setCursor(0, 1);
       //lcd.print(F("PAS RTC"));
    Serial.println("PAS RTC");
    errH();
    while (1); // Stop si pas de RTC trouvé
  }

  if (!aht.begin()) {
       //lcd.setCursor(0, 1);
       //lcd.print(F("PAS AHT"));
        while (1); // Bloque le programme en cas d'échec
    }

  if (!bmp.begin(0x77)) {
       //lcd.setCursor(0, 1);
       //lcd.print(F("PAS BMP"));
    Serial.println(F("PAS BMP280"));
    errC();
    while (1); // Stop si pas de BMP280 trouvé
  }

  

  if (!SD.begin(SD_CS_PIN)) {
      carteP();
    return;
  }
  Serial.println(F("OK SD"));
    //lcd.setCursor(0, 1);
    //lcd.print(F("SD OK"));

  Conf tempConf = {0};
  loadConf();
  if (memcmp(&conf, &tempConf, sizeof(Conf)) == 0) {
    resetConf();
  }
  printConf();
  printTime();

  checkStartMode(); // Détermine le mode initial
  modeStart = millis(); // Enregistre le temps de démarrage
}

// Vérifier si la RTC est connecté
bool rtcConnected() {
  Wire.beginTransmission(DS1307_ADDRESS);
  if (Wire.endTransmission() == 0) {
    return true; 
  } else {
    return false;
  }
}
