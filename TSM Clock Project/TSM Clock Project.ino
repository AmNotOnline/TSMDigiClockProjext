#include <WiFiManager.h>
#include <WiFi.h> 
#include <Audio.h>
#include <SD.h>
#include <FS.h>

#define DEBUG 35

WiFiManager wm;

void setup() {
  Serial.begin(9600);
  pinMode(DEBUG, OUTPUT);
  wm.autoConnect("TSM-klok", "TSM-klok");
  digitalWrite(DEBUG, HIGH);
}

void loop() {
  // put your main code here, to run repeatedly:

}