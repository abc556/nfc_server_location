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

#include <SPI.h>
#include <PN532_SPI.h>
#include <PN532.h>
#include <NfcAdapter.h>

#define TAG_NOT_PRESENT "TAG_NOT_PRESENT"
#define TAG_NULL_CONTENT "NULL"

// nfc configuration //
#define READER_COUNT 2
///////////////////////

extern NfcAdapter nfc1;
extern NfcAdapter nfc2;

extern String NFCs_uid [READER_COUNT];
extern String NFCs_content [READER_COUNT];

void nfc_begin();
void nfc_read_all();
void printNFCsContents();
void nfc_read(NfcAdapter &nfc, int reader_num);
boolean nfc_write(NfcAdapter &nfc, String content);

#endif