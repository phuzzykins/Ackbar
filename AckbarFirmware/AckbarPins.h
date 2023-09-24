#ifndef __ACKBAR_PINS_H__
#define __ACKBAR_PINS_H__

#include "Arduino.h"

#define STP_MS1   17
#define STP_MS2   18
#define STP_MS3   33
#define STP_PDN   34
#define STP_DIAG  35
#define STP_EN    38
#define STP_STEP  39
#define STP_DIR   40

#define STP_UART_TX  STP_MS3
#define STP_UART_RX  STP_PDN

#define EPD_DC    14
#define EPD_CS    10
#define EPD_BUSY  16
#define SRAM_CS   -1
#define EPD_RESET 15
#define EPD_SPI   &SPI

#define TOF_XSHUT 7

#define GNSS_INT  6

#define HALL      41

#define RGB_BUILTIN 21

#endif // __ACKBAR_PINS_H__