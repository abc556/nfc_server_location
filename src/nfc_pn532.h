#ifndef __NFC_PN532_H__
#define __NFC_PN532_H__

#define CS1 4
#define CS2 5
#define CS3 21
#define CS4 22

#define NFC_TIMEOUT 250

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
String nfc_read(NfcAdapter adapter);
String nfc_reading_combine();


#endif