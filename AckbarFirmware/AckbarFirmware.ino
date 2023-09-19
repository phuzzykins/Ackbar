
#include "Ackbar.h"


/*
Adafruit_SSD1608 epdDevice(200, 200, EPD_DC, EPD_RESET, EPD_CS, SRAM_CS, EPD_BUSY, EPD_SPI);
*/

Ackbar trap;

void setup() {
  Serial.begin(115200);
}

void loop() {
  trap.doWork();
}
