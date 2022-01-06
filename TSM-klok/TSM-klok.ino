#include <Adafruit_NeoPixel.h>
#include <ArduinoJson.h> // installeer via package manager of https://github.com/pkourany/ArduinoJson
#include <EEPROM.h>
#include <WiFi.h>
#include <AsyncTCP.h> // zip
#include <ESPAsyncWebServer.h> // zip
#include <SPIFFS.h> // tool, zie README.txt
#include <Audio.h>  // zip, NIET DEFAULT AUDIO.H VAN ARDUINO 
#include <SD.h>
#include <FS.h>

// I/O pinnen voor Audio en SD
#define SD_CS          5
#define SPI_MOSI      23
#define SPI_MISO      19
#define SPI_SCK       18
#define I2S_DOUT      25
#define I2S_BCLK      27
#define I2S_LRC       26

#define AUDIO_DEFAULTVOLUME 21   // 0..21
#define AUDIO_ISMONO        true // Maar één speaker aanwezig

// NeoPixel setup
#define LED_PIN       33
#define LED_COUNT     60 // Hoeveelheid LEDs


String ssid = "", psk = "", dt = "auto", color = "auto", intensity = "auto";
byte volume = 100;
bool connected = false;

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
Audio audio;
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

void musicStream(String file, byte volume, byte balance) {
  audio.setVolume(volume);
  audio.setBalance(balance);
  audio.connecttoFS(SD, file);
  audio.loop();     // loop over de muziekdata, moet in loop?
}

void testVolume(byte vol) {
  // TODO: test volume
}

void testLed(String color, byte inten) {
  // TODO: test leds
}


void writeConfig(JsonVariant doc) {
  /* STORE CONFIGURATION DATA IN EEPROM */
  
  // read data from json
  ssid = doc["ssid"].as<String>();
  psk = doc["psk"].as<String>();
  dt = doc["dt"].as<String>();
  intensity = (byte)doc["intensity"].as<unsigned int>();
  color =  doc["color"].as<String>();
  volume = (byte)doc["volume"].as<unsigned int>();
  
  String config;
  serializeJson(doc, config); // configuration is now stored in config
  doc.clear(); // free memory

  // clear EEPROM
  for (int i = 0; i < 512; i++) EEPROM.write(i, 0);

  // compute and store length
  int len = config.length();
  byte upper = len >> 8;
  byte lower = len & 0xff;
  EEPROM.write(0, upper);
  EEPROM.write(1, lower);
  
  for (int i = 0; i < len; i++) EEPROM.write(2 + i, config.charAt(i)); // write config

  EEPROM.commit();
}

void readConfig() {
  /* READ CONFIGURATION DATA FROM EEPROM */ 
  
  int len = (EEPROM.read(0) << 8) | EEPROM.read(1); // calculate size
  
  // read data
  String data = "";
  for (int i = 0; i < len; i++) {
    char in = EEPROM.read(2 + i);
    if (in == 0) break;
    data += in;
  }

  // extract data
  DynamicJsonDocument doc(512);
  deserializeJson(doc, data);
  ssid = doc["ssid"].as<String>();
  psk = doc["psk"].as<String>();
  dt = doc["dt"].as<String>();
  intensity = (byte)doc["intensity"].as<unsigned int>();
  color =  doc["color"].as<String>();
  volume = (byte)doc["volume"].as<unsigned int>();
  
  doc.clear();
  
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
    String content = String((char*)data);
    DynamicJsonDocument doc(512);
    deserializeJson(doc, content);
    if (doc.containsKey("config")) {
      Serial.println("new configuration received");
      String psk_ = psk;
      String ssid_ = ssid;
      writeConfig(doc["config"]);
      if ((psk != "" || ssid != "") && (psk != psk_ || ssid != ssid_)) {
        ws.textAll("{\"info\":\"ESP gaat rebooten. Wacht een paar seconden en refresh deze pagina\"}");
        ESP.restart();
      }
    }
    if (doc.containsKey("testVolume")) testVolume((byte)doc["testVolume"]["volume"]);
    if (doc.containsKey("testLed")) testLed(String((const char*)doc["testLed"]["color"]), (byte)doc["testLed"]["intensity"]);
  }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      ws.textAll("{\"ssid\":\"" + ssid + "\",\"psk\":\"" + psk + "\"}");
      break;
    case WS_EVT_DATA:
      handleWebSocketMessage(arg, data, len);
      break;
    default:
      break;
  }
}

void setup() {
  Serial.begin(9600);
  EEPROM.begin(512);

// Audio & SD card setup
  pinMode(SD_CS, OUTPUT);
  digitalWrite(SD_CS, HIGH);

  SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);
  SD.begin (SD_CS);
  audio.setPinout (I2S_BCLK, I2S_LRC, I2S_DOUT);
  audio.setVolume (AUDIO_DEFAULTVOLUME);
  audio.setBalance(0);  // Stereo perfect in het midden
  audio.forceMono (AUDIO_ISMONO);

  strip.begin();
  strip.show();         // Init alle LEDs als uit

  if(!SPIFFS.begin(true)){
    Serial.println("Couldn't mount SPIFFS");
    while (1);
  }
  Serial.println("SPIFFS mounted");

  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP("TSM-klok", NULL);
  /*WiFi.softAPConfig(
    IPAddress(192, 168, 4, 1),
    IPAddress(192, 168, 4, 1),
    IPAddress(255, 255, 255, 0)
  );*/
  Serial.println("AP mode started");

  File file = SPIFFS.open("/index.html", FILE_READ);
  if (!file) {
    Serial.println("index.html not found");
    while (1);
  }
  file = SPIFFS.open("/style.css", FILE_READ);
  if (!file) {
    Serial.println("style.css not found");
    while (1);
  }
  file = SPIFFS.open("/script.js", FILE_READ);
  if (!file) {
    Serial.println("script.js not found");
    while (1);
  }
  file = SPIFFS.open("/icon.png", FILE_READ);
  if (!file) {
    Serial.println("icon.png not found");
    while (1);
  }
  
  ws.onEvent(onEvent);
  server.addHandler(&ws);
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", "text/html");
  });
  Serial.println("HTML loaded");
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/style.css", "text/css");
  });
  Serial.println("CSS loaded");
  server.on("/script.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/script.js", "text/js");
  });
  Serial.println("JS loaded");
  server.on("/icon.png", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/icon.png", "image/png");
  });
  Serial.println("PNG loaded");

  server.begin();

  readConfig();
  if (psk == "" || ssid == "") return;
  connected = WiFi.begin(ssid.c_str(), psk.c_str());
  if (connected) {
    Serial.println("reached target network");
    ws.textAll("{\"info\":\"Wifi-connectie successvol\"}");
  } else {
    ws.textAll("{\"info\":\"Kan geen verbinding maken met het netwerk\"}");
  }
}

void loop() {
  //while (!connected);
}
