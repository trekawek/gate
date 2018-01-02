#include <TinyGPS.h>
#include <SoftwareSerial.h>
#include <EEPROM.h>

#define SIGNAL_REPEAT 30

#define RF_TRANSMIT_PIN 5
#define GPS_RX_PIN 3
#define GPS_TX_PIN 4

extern const int SIGNALS_SIZE;
extern const int SIGNALS[][256];
extern const float HOME_LAT;
extern const float HOME_LON;

SoftwareSerial gpsSerial(GPS_RX_PIN, GPS_TX_PIN);
TinyGPS gps;
boolean atHome;

void setup() {
   //initPersistence();
  
   pinMode(RF_TRANSMIT_PIN, OUTPUT);
   pinMode(LED_BUILTIN, OUTPUT);
   
   digitalWrite(RF_TRANSMIT_PIN, LOW);
   digitalWrite(LED_BUILTIN, LOW);

   Serial.begin(9600);
   gpsSerial.begin(9600);

   atHome = loadBoolean();
}

void loop() {
  long dist = getDistance();
  updateAtHome(dist);

  boolean doTransmit = false;
  long delaySec = 0;

  if (dist == -1 && atHome) { // home garage
    doTransmit = true;
    delaySec = 0;
  } else if (dist == -1) { // other garage
    doTransmit = false;
    delaySec = 10;
  } else if (0 <= dist && dist < 100) { // <100m
    doTransmit = true;
    delaySec = 0;
  } else if (100 <= dist && dist < 1e3) { // 100m...1km
    doTransmit = false;
    delaySec = 5;
  } else if (1e3 <= dist) { // > 5km
    doTransmit = false;
    delaySec = 30 * dist / 1e3; // 30s delay per 1km
  }

  if (doTransmit) {
    transmitSignals();
  }
  if (delaySec > 0) {
    delay(delaySec * 1000);
  }
}

long getDistance() {
  float flat, flon;
  unsigned long age;
  readGps();
  gps.f_get_position(&flat, &flon, &age);
  if (flat == TinyGPS::GPS_INVALID_F_ANGLE) {
    return -1;
  }
  return (long) TinyGPS::distance_between(flat, flon, HOME_LAT, HOME_LON);
}

void updateAtHome(long dist) {
  boolean newAtHome = atHome;
  if (dist >= 0 && dist < 100) {
    newAtHome = true;
  } else if (dist > 150) {
    newAtHome = false;
  }
  
  if (atHome != newAtHome) {
    atHome = newAtHome;
    persistBoolean(atHome);
  }
}

void readGps() {
  bool ready = false;
    while (!ready) {
    if (gpsSerial.available()) {
      char c = gpsSerial.read();
      if (gps.encode(c)) {
        ready = true;
      }
    }
  }
}

void transmitSignals() {
  for (int i = 0; i < SIGNALS_SIZE; i++) {
    for (int j = 0; j < SIGNAL_REPEAT; j++) {
      for (int k = 0; SIGNALS[i][k] != 0; k += 2) {
        digitalWrite(RF_TRANSMIT_PIN, HIGH);
        digitalWrite(LED_BUILTIN, HIGH);
        delayMicroseconds(SIGNALS[i][k]);
        digitalWrite(RF_TRANSMIT_PIN, LOW);
        digitalWrite(LED_BUILTIN, LOW);
        delayMicroseconds(SIGNALS[i][k + 1]);
      }
    }
  }
}

