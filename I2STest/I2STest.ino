#include <Audio.h>
#include <SD.h>
#include <FS.h>

// I/O pinnen voor SD
#define SPI_MISO      5
#define SPI_SCK       18
#define SPI_MOSI      19
#define SD_CS         21
// 1/2 GND niet aangesloten
// 5V      niet aangesloten
// 3.3V    wel aangesloten 

// I/O voor Amplifier
#define I2S_LRC       27
#define I2S_BCLK      26
#define I2S_DOUT      25
// GAIN pin op MAX98357A chip naar GND
// SD pin naar Vin / 5V
// Vin naar 5V

#define AUDIO_DEFAULTVOLUME 5   // 0..21
#define AUDIO_ISMONO        true // Maar één speaker aanwezig

String file  = "BBC - Bell Long.mp3"; // Pad naar bestand dat moet worden afgespeeld



Audio audio;

File root; // ⇐⇓DEBUGGING
void listDirectory(File dir, byte levels = 0);

void setup() {
  Serial.begin(115200);
  Serial.print("\n\n");
  // Audio & SD card setup
  pinMode(SD_CS, OUTPUT);
  digitalWrite(SD_CS, HIGH);

  SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);
  if (!SD.begin(SD_CS)) {
    Serial.println("SD card failed to open on boot or not present");
  }

  audio.setPinout (I2S_BCLK, I2S_LRC, I2S_DOUT);
  audio.setVolume (AUDIO_DEFAULTVOLUME);
  audio.setBalance(0);  // Stereo perfect in het midden
  audio.forceMono (AUDIO_ISMONO);
  audio.connecttoFS(SD, file.c_str());

// DEBUGGING
  root = SD.open("/");
  listDirectory(root, 0);
}

void loop() {
  audio.loop();                               // Functie moet zo snel mogelijk geloopt worden om muziek af te spelen
  audio.setVolume((analogRead(2) >> 6) / 3);  // Optimale? conversie van 0xFFF naar 21 voor potentiometer als volume-knop op bord
}                                             // ADC op esp32 zijn 12 bits

void audio_eof_mp3(const char *info) { // Wordt uitgevoerd wanneer sound file klaar is

}

// OOK DEBUGGING: Print alle files op SD naar serial
void listDirectory(File dir, byte levels) {
  while (true) {
    File entry = dir.openNextFile();
    if (!entry) { // Geen verdere bestanden
      break;
    }
    for (byte i = 0; i < levels; i++) Serial.print('\t');

    Serial.print(entry.name());
    if (entry.isDirectory()) {
      Serial.println("/");
      listDirectory(entry, levels + 1);
    } else { // Voor bestanden
      Serial.print("\t\t\t\t\t\t\t\t");
      Serial.println(entry.size(), DEC);
    }
    entry.close();
  }
}
