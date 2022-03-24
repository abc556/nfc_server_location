#ifndef __NFC_PN532_H__
#define __NFC_PN532_H__

#define CS1 4
#define CS2 5
#define CS3 16
#define CS4 13
#define CS5 14
#define CS6 21
#define CS7 22
#define CS8 25
#define CS9 26
#define CS10 27

#define NFC_TIMEOUT 100

extern const int READER_COUNT;

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

extern NfcAdapter nfc1;
extern NfcAdapter nfc2;
extern NfcAdapter nfc3;
extern NfcAdapter nfc4;
extern NfcAdapter nfc5;
// extern NfcAdapter nfc6;
// extern NfcAdapter nfc7;
// extern NfcAdapter nfc8;
// extern NfcAdapter nfc9;
// extern NfcAdapter nfc10;


extern String nfc_uid;
extern String nfc_content;


void nfc_begin();
void nfc_loop();
void nfc_read(NfcAdapter &nfc, int reader_num);
void nfc_write(NfcAdapter &nfc, int reader_num, String content);


#endif