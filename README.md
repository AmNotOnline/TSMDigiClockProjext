# TSM Digital Clock Project

## Hardware
- *WS2812B RGB LED strip:* Voor illuminatie letters, zie FastLED library
- *SD-Card Reader:* Om externe MP3's om af te spelen als alarms
- *I²S module:* Om MP3's te versterken en af te spelen
- *Externe RTC module:* Houdt de tijd bij wanneer er geen internet beschikbaar is

## Software
- *Tijd:* Wordt regelmatig over het internet opgevraagd (+ correcte voor zomer- en wintertijd)
- *Instellingen:* ESP32 opent netwerk waarop via een computer meerdere instellingen veranderd kunnen worden. Waaronder:
    - Alarm
    - Manuele tijd instellen
    - Intensiteit en kleur van leds veranderen (eventueel schema laten instellen voor verandering van intensiteit en kleur over de dag)
