# Werkinstructie — Teensy 4.1 sACN/DMX LED Node

Deze instructie beschrijft hoe je de firmware bouwt, op de Teensy 4.1 zet, configureert en functioneel test.

## 1. Doel

Het systeem:

- stuurt 10 WS2812B-uitgangen aan
- ontvangt sACN (E1.31) via Ethernet
- ondersteunt 8 DMX-poorten (input/output per poort)
- werkt in `pixel` mode of `effect` mode

## 2. Benodigdheden

- Teensy 4.1
- WS2812B strips (bij voorkeur eerst 1 strip voor test)
- 5V voeding voor LED’s (niet via Teensy voeden)
- Ethernetverbinding (sACN netwerk)
- SD-kaart (FAT32)
- USB-kabel
- PC met Python 3 en toegang tot de projectmap

## 3. Voorbereiding software

Open een terminal in de projectmap:

`c:\Users\mitchel\Documents\Dev\teensy-app`

Installeer PlatformIO (eenmalig):

`python -m pip install --user platformio`

## 4. Build uitvoeren

Snelle testbuild (30 LEDs/strip):

`python -m platformio run -e teensy41_test`

Productiebuild (300 LEDs/strip):

`python -m platformio run -e teensy41`

Als de build slaagt, verschijnt firmware in `.pio/build/...`.

## 5. Firmware uploaden

Upload productieversie:

`python -m platformio run -t upload -e teensy41`

Seriële monitor starten:

`python -m platformio device monitor`

Controleer op startup logs zoals mode, LED-aantal, universes en netwerkstatus.

## 6. SD-configuratie plaatsen

1. Neem `data/config.json` als basis.
2. Kopieer dit bestand naar de root van de SD-kaart als `config.json`.
3. Plaats SD-kaart in Teensy.
4. Herstart de node.

Belangrijk:

- Wijzig je `mode` (`pixel` of `effect`), dan altijd herstarten.
- Zonder geldige SD-config valt firmware terug op embedded defaults.

## 7. Aansluiten hardware (minimum veilig)

Voor eerste test:

1. Sluit 1 WS2812B strip data aan op pin volgens `docs/PINMAP.md`.
2. Verbind GND van LED-voeding met GND van Teensy.
3. Gebruik level shifter (74HCT245/SN74AHCT125) tussen Teensy en WS2812 data.
4. Sluit Ethernet aan.

Let op:

- LED-stroom altijd extern dimensioneren.
- Geen hoge LED-stroom via Teensy printsporen.

## 8. Functionele test — Pixel mode

1. Zet in `config.json`: `"mode": "pixel"`.
2. Herstart node.
3. Stuur sACN universes 1 t/m 18 vanaf je controller/software.
4. Controleer:
   - kleuren volgen inkomende RGB-data
   - mapping loopt lineair door over stripgrenzen
   - bij verlies van data blijft laatste frame stabiel

## 9. Functionele test — Effect mode

1. Zet in `config.json`: `"mode": "effect"`.
2. Herstart node.
3. Stuur universe 100 kanaaldata:
   - per strip 6 kanalen: `R,G,B,Program,Dimmer,Speed`
4. Controleer:
   - programmakeuze werkt op kanaal 4
   - dimmer werkt als master brightness
   - snelheid beïnvloedt animatie
   - VirtualAll-effect loopt vloeiend over alle strips

## 10. DMX-poorten testen

1. Configureer `dmx.ports[]` per poort als `input` of `output`.
2. Herstart node.
3. Test elke poort afzonderlijk:
   - output: check DMX-signaal op fixture/tester
   - input: check activiteit en timeouts in gedrag/logs
4. Herhaal voor alle 8 poorten.

## 11. Integratie en duurtest

Volg de uitgebreide checklist in:

`docs/INTEGRATION_SOAK_TEST.md`

Doel:

- 24 uur stabiel draaien zonder crash/reset
- sACN + LED + DMX gelijktijdig verifiëren

## 12. Oplossen van problemen

### Build faalt

- Controleer Python/PlatformIO installatie
- Run opnieuw: `python -m platformio run -e teensy41_test`

### Geen LED-output

- Controleer voeding en GND
- Controleer pinmapping in `config.json` en `docs/PINMAP.md`
- Controleer level shifter

### Geen sACN-reactie

- Controleer IP/subnet/gateway in `config.json`
- Controleer of sACN bron de juiste universes uitstuurt
- Controleer Ethernet link status in serial log

### Config wordt niet geladen

- Bestand heet exact `config.json`
- Staat in root van SD-kaart
- SD-kaart is FAT32

## 13. Oplevercheck

Voor vrijgave minimaal:

- `teensy41_test` build geslaagd
- `teensy41` build geslaagd
- Pixel mode functioneel
- Effect mode functioneel
- DMX basisfunctionaliteit op alle 8 poorten getest
- Soak test uitgevoerd volgens checklist
