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

extern int reader_count;

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
void nfc_read();
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