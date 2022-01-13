#include <Audio.h>
#include <ESP.h>
#include <FS.h>

// I/O pinnen voor Audio en SD
#define SD_CS         23
#define SPI_MOSI      18
#define SPI_MISO      16
#define SPI_SCK       17

#define I2S_DOUT      25
#define I2S_BCLK      26
#define I2S_LRC       27

#define AUDIO_DEFAULTVOLUME 21   // 0..21
#define AUDIO_ISMONO        true // Maar één speaker aanwezig


const String file = "Mother Earth - MOTHER.mp3";


Audio audio;

void setup() {
  Serial.begin(115200);

  // Audio & SD card setup
  pinMode(SD_CS, OUTPUT);
  digitalWrite(SD_CS, HIGH);

  SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);
  SD.begin (SD_CS);
  audio.setPinout (I2S_BCLK, I2S_LRC, I2S_DOUT);
  audio.setVolume (AUDIO_DEFAULTVOLUME);
  audio.setBalance(0);  // Stereo perfect in het midden
  audio.forceMono (AUDIO_ISMONO);
}

void loop() {
  audio.connecttoFS(SD, file.c_str());

}
