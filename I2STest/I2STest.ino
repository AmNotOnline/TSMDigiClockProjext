#include <Audio.h>
#include <SD.h>
#include <FS.h>

// I/O pinnen voor Audio en SD
#define SD_CS         21
#define SPI_MOSI      19
#define SPI_MISO      5
#define SPI_SCK       18

#define I2S_DOUT      25
#define I2S_BCLK      26
#define I2S_LRC       27

#define AUDIO_DEFAULTVOLUME 12   // 0..21
#define AUDIO_ISMONO        true // Maar één speaker aanwezig

String file  = "BBC - Bell Long.mp3";



Audio audio;
File root;

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

  root = SD.open("/");
  
  audio.setPinout (I2S_BCLK, I2S_LRC, I2S_DOUT);
  audio.setVolume (AUDIO_DEFAULTVOLUME);
  audio.setBalance(0);  // Stereo perfect in het midden
  audio.forceMono (AUDIO_ISMONO);
  audio.connecttoFS(SD, file.c_str());

  listDirectory(root, 0);
}

void loop() {
  byte ch;
  audio.loop();
}

void audio_eof_mp3(const char *info) {

}

void listDirectory(File dir, byte levels) {
  while (true) {
    File entry = dir.openNextFile();
    if (!entry) { // No more files
      break;
    }
    for (byte i = 0; i < levels; i++) Serial.print('\t');
    
    Serial.print(entry.name());
    if (entry.isDirectory()) {
      Serial.println("/");
      listDirectory(entry, levels + 1);
    } else { // for files
      Serial.print("\t\t");
      Serial.println(entry.size(), DEC);
    }
    entry.close();
  }
}
