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

#define AUDIO_DEFAULTVOLUME 5    // 0..21
#define AUDIO_ISMONO        true // Maar één speaker aanwezig
#define AUDIO_ENABLE        14   // Amp chip disable
#define AUDIO_VOLPOT        2


Audio audio;

int vol;

TaskHandle_t play;
void startTask(void);

void playAudio(void * parameter) {
  digitalWrite(AUDIO_ENABLE, HIGH);
  Serial.println("Audio enabled");
  while (1) {
    audio.loop();
    vol = analogRead(AUDIO_VOLPOT) / 400;
    Serial.println(vol);
    audio.setVolume(vol);
  }
}

void audio_eof_mp3(const char *info) { // Wordt uitgevoerd wanneer sound file klaar is
  digitalWrite(AUDIO_ENABLE, LOW);
  Serial.println("Audio disabled");
  if (play != NULL) vTaskSuspend(play);
}

void setup() {
  Serial.begin(115200);
  Serial.print("\n\n\n");

  pinMode(AUDIO_VOLPOT, INPUT);
  pinMode(SD_CS, OUTPUT);
  digitalWrite(SD_CS, HIGH);

  SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);
  if (!SD.begin(SD_CS)) {
    Serial.println("SD card failed to open on boot or not present");
  }
  audio.setPinout (I2S_BCLK, I2S_LRC, I2S_DOUT);
  audio.setVolume (AUDIO_DEFAULTVOLUME);
  audio.setBalance(0);  
  audio.forceMono (AUDIO_ISMONO);
  pinMode(AUDIO_ENABLE, OUTPUT);
  digitalWrite(AUDIO_ENABLE, LOW);

  Serial.println("Waiting 2s");
  delay(2000);
  Serial.println("Audio task started");
  startTask();
}

void startTask(void) {
  audio.connecttoFS(SD, "BBC - Bell Long.mp3");

  static bool created;

  if (!created) {
    xTaskCreatePinnedToCore(
      playAudio,   /* Function to implement the task */
      "audioLoop", /* Name of the task */
      5000,        /* Stack size in words */
      NULL,        /* Task input parameter */
      1,           /* Priority of the task */
      &play,       /* Task handle. */
      0
    );
    created = true;
  } else {
    vTaskResume(play);
  }
}

void loop() {
}
