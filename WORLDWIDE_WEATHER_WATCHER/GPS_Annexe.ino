#include <SoftwareSerial.h>
#include <TinyGPS++.h>

static const int RXPin = 8, TXPin = 9;
static const uint32_t GPSBaud = 9600; // Vérifie la vitesse de ton module GPS

SoftwareSerial gpsSerial(RXPin, TXPin);
TinyGPSPlus gps;

void setup() {
    Serial.begin(115200); // Moniteur Série
    gpsSerial.begin(GPSBaud);
    Serial.println("GPS en attente de signal...");
}

void loop() {
    while (gpsSerial.available()) {
        gps.encode(gpsSerial.read());
        
        if (gps.location.isUpdated()) {
            Serial.print("Latitude: ");
            Serial.print(gps.location.lat(), 6);
            Serial.print(" Longitude: ");
            Serial.println(gps.location.lng(), 6);
        }
    }
}
