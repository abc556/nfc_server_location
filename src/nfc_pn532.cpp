#include "nfc_pn532.h"
#include "mqtt.h"

// nfc configuration //
PN532_SPI pn532spi1(SPI, CS1);
PN532_SPI pn532spi2(SPI, CS2);
NfcAdapter nfc1 = NfcAdapter(pn532spi1);
NfcAdapter nfc2 = NfcAdapter(pn532spi2);
////////////////////////

String NFCs_uid [READER_COUNT];
String NFCs_content [READER_COUNT];

void nfc_begin() {
  nfc1.begin();
  nfc2.begin();
}

void nfc_read_all() {
  nfc_read(nfc1, 1);
  nfc_read(nfc2, 2);
}

void printNFCsContents() {
  String print = "";
    for (int i = 0; i < READER_COUNT; i++) {
        print += String(i+1) + ":" + NFCs_content[i] + "\t";
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