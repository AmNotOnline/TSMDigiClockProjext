# TSM Digital Clock Project

## Hardware
- *ESP32:* Core van het project
- *WS2812B RGB LED strip:* Voor illuminatie letters, zie Adafruit Neopixel library
- *SD-Card Reader:* Om externe MP3's om af te spelen als alarms
- *I²S versterker module (MAX98357?):* Om MP3's te versterken en af te spelen
- *Externe RTC module:* Houdt de tijd bij wanneer er geen internet beschikbaar is

## Software
- *Tijd:* Wordt regelmatig over het internet opgevraagd (+ correcte voor zomer- en wintertijd) Wordt ook verder bijgehouden door de RTC wanneer het internet niet beschikbaar is of uitvalt
- *Instellingen:* ESP32 opent netwerk waarop via een computer meerdere instellingen veranderd kunnen worden. Waaronder:
    - Alarm  
        - Wordt ingesteld via netwerk
        - Geluidsbestand wordt van SD kaart naar versterker module gestreamd
    - Manuele tijd instellen
    - Intensiteit en kleur van leds veranderen (eventueel schema laten instellen voor verandering van intensiteit en kleur over de dag)
