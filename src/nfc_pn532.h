#ifndef __NFC_PN532_H__
#define __NFC_PN532_H__

#define CS1 4
#define CS2 5
#define CS3 12
#define CS4 13
#define CS5 14
#define CS6 21
#define CS7 22
#define CS8 25
#define CS9 26
#define CS10 27
#define CS11 32
#define CS12 33

#define NFC_TIMEOUT 500

#include <SPI.h>
#include <PN532_SPI.h>
#include <PN532.h>
#include <NfcAdapter.h>

// #include <Wire.h>
// #include <PN532_I2C.h>
// #include <PN532.h>
// #include <NfcAdapter.h>

// PN532_I2C pn532_i2c(Wire);
// NfcAdapter nfc = NfcAdapter(pn532_i2c);

void nfc_begin();
void nfc_loop();

#endif