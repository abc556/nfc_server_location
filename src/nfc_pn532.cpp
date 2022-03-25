#include "nfc_pn532.h"
#include "mqtt.h"

// nfc configuration //
PN532_SPI pn532spi1(SPI, CS1);
PN532_SPI pn532spi2(SPI, CS2);
PN532_SPI pn532spi3(SPI, CS3);
PN532_SPI pn532spi4(SPI, CS4);
PN532_SPI pn532spi5(SPI, CS5);
PN532_SPI pn532spi6(SPI, CS6);
PN532_SPI pn532spi7(SPI, CS7);
PN532_SPI pn532spi8(SPI, CS8);
PN532_SPI pn532spi9(SPI, CS9);
PN532_SPI pn532spi10(SPI, CS10);
NfcAdapter nfc1 = NfcAdapter(pn532spi1);
NfcAdapter nfc2 = NfcAdapter(pn532spi2);
NfcAdapter nfc3 = NfcAdapter(pn532spi3);
NfcAdapter nfc4 = NfcAdapter(pn532spi4);
NfcAdapter nfc5 = NfcAdapter(pn532spi5);
NfcAdapter nfc6 = NfcAdapter(pn532spi6);
NfcAdapter nfc7 = NfcAdapter(pn532spi7);
NfcAdapter nfc8 = NfcAdapter(pn532spi8);
NfcAdapter nfc9 = NfcAdapter(pn532spi9);
NfcAdapter nfc10 = NfcAdapter(pn532spi10);
void nfc_begin() {
  nfc1.begin();
  nfc2.begin();
  nfc3.begin();
  nfc4.begin();
  nfc5.begin();
  nfc6.begin();
  nfc7.begin();
  nfc8.begin();
  nfc9.begin();
  nfc10.begin();
}
void nfc_read_all() {
  nfc_read(nfc1, 1);
  nfc_read(nfc2, 2);
  nfc_read(nfc3, 3);
  nfc_read(nfc4, 4);
  nfc_read(nfc5, 5);
  nfc_read(nfc6, 6);
  nfc_read(nfc7, 7);
  nfc_read(nfc8, 8);
  nfc_read(nfc9, 9);
  nfc_read(nfc10, 10);
}
////////////////////////

String NFCs_uid [READER_COUNT];
String NFCs_content [READER_COUNT];



void printNFCsContents() {
  String print = "";
    for (int i = 0; i < READER_COUNT; i++) {
        print += String(i+1) + ":" + NFCs_content[i] + "\r\n";
        // if (i%3 == 0) { print += "\n"; }
    }
    Serial.println(print);
}

void nfc_read(NfcAdapter &nfc, int reader_num) {
  int reader_idx = reader_num - 1;
  if (nfc.tagPresent(NFC_TIMEOUT)) {
    NfcTag tag = nfc.read();
    if (tag.hasNdefMessage()) {
      NFCs_uid[reader_idx] = tag.getUidString();
      NdefMessage msg = tag.getNdefMessage();
      int recordCount = msg.getRecordCount();
      for (int i = 0; i < recordCount; i++) {
        NdefRecord record = msg.getRecord(i);
        int payloadLength = record.getPayloadLength();
        byte payload[payloadLength];
        record.getPayload(payload); 
        String nfc_content = "";
        if (payloadLength > 4) {
          for (int c = 0; c < payloadLength; c++) {
            nfc_content += (char)payload[c];
          }
        } else {
          nfc_content = TAG_NULL_CONTENT;
        }
        NFCs_content[reader_idx] = nfc_content;
      }
    } else {
      NFCs_uid[reader_idx] = TAG_NULL_CONTENT;
      NFCs_content[reader_idx] = TAG_NULL_CONTENT;
    }
  } else {
    NFCs_uid[reader_idx] = TAG_NOT_PRESENT;
    NFCs_content[reader_idx] = TAG_NOT_PRESENT;
    nfc.begin();
  }
}

boolean nfc_write(NfcAdapter &nfc, String content) {
  NdefMessage msg = NdefMessage();
  msg.addTextRecord(content);
  if (nfc.tagPresent(NFC_TIMEOUT)) {
    if(nfc.write(msg)){
      return true;
    }else{
      return false;
    }
  }
  return false;
}