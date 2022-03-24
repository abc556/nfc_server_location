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
extern NfcAdapter nfc6;
extern NfcAdapter nfc7;
extern NfcAdapter nfc8;
extern NfcAdapter nfc9;
extern NfcAdapter nfc10;

extern String nfc1_uid;
extern String nfc2_uid;
extern String nfc3_uid;
extern String nfc4_uid;
extern String nfc5_uid;
extern String nfc6_uid;


void nfc_begin();
void nfc_loop();
String* nfc_read(NfcAdapter &nfc);
void nfc_write(NfcAdapter &nfc, int reader_num, String content);
String nfc1_getuid();
String nfc2_getuid();
String nfc3_getuid();
String nfc4_getuid();
String nfc5_getuid();
String nfc6_getuid();
String nfc1_read();
String nfc2_read();
String nfc3_read();
String nfc4_read();
String nfc5_read();
String nfc6_read();
void nfc1_write(String content);
void nfc2_write(String content);
void nfc3_write(String content);
void nfc4_write(String content);
void nfc5_write(String content);
void nfc6_write(String content);

#endif