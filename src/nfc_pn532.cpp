#include "nfc_pn532.h"
#include "mqtt.h"

const int READER_COUNT = 10;

String nfc_uid = "";
String nfc_content = "";

PN532_SPI pn532spi1(SPI, CS1);
PN532_SPI pn532spi2(SPI, CS2);
PN532_SPI pn532spi3(SPI, CS3);
PN532_SPI pn532spi4(SPI, CS4);
PN532_SPI pn532spi5(SPI, CS5);
// PN532_SPI pn532spi6(SPI, CS6);
// PN532_SPI pn532spi7(SPI, CS7);
// PN532_SPI pn532spi8(SPI, CS8);
// PN532_SPI pn532spi9(SPI, CS9);
// PN532_SPI pn532spi10(SPI, CS10);
NfcAdapter nfc1 = NfcAdapter(pn532spi1);
NfcAdapter nfc2 = NfcAdapter(pn532spi2);
NfcAdapter nfc3 = NfcAdapter(pn532spi3);
NfcAdapter nfc4 = NfcAdapter(pn532spi4);
NfcAdapter nfc5 = NfcAdapter(pn532spi5);
// NfcAdapter nfc6 = NfcAdapter(pn532spi6);
// NfcAdapter nfc7 = NfcAdapter(pn532spi7);
// NfcAdapter nfc8 = NfcAdapter(pn532spi8);
// NfcAdapter nfc9 = NfcAdapter(pn532spi9);
// NfcAdapter nfc10 = NfcAdapter(pn532spi10);

String mqtt_data = "";




void nfc_begin(){
    nfc1.begin();
    nfc2.begin();
    nfc3.begin();
    nfc4.begin();
    nfc5.begin();
    // nfc6.begin();
    // nfc7.begin();
    // nfc8.begin();
    // nfc9.begin();
    // nfc10.begin();
}


void nfc_read(NfcAdapter &nfc, int reader_num){
  if (nfc.tagPresent(NFC_TIMEOUT)) {
    NfcTag tag = nfc.read();
    if (tag.hasNdefMessage()) {
      nfc_uid = tag.getUidString();
      nfc_content = "READ_FAILED";
      NdefMessage message = tag.getNdefMessage();
      int recordCount = message.getRecordCount();
      for (int i = 0; i < recordCount; i++) {
        NdefRecord record = message.getRecord(i);
        int payloadLength = record.getPayloadLength();
        byte payload[payloadLength];
        record.getPayload(payload);
        nfc_content = "";
        for (int c = 0; c < payloadLength; c++) {
          nfc_content += (char)payload[c];
        }
      }
    }
  } else {
    nfc_uid = "NO_NFC";
    nfc_content = "NO_NFC";
  }
  Serial.println(nfc_content);
}

void nfc_write(NfcAdapter &nfc, int reader_num, String content) {
  if (nfc.tagPresent(NFC_TIMEOUT)) {
    NdefMessage message = NdefMessage();
    message.addTextRecord(content);
    if(nfc.write(message)){
      Serial.println("Reader " + String(reader_num) + ": write success");
    }else{
      Serial.println("Reader " + String(reader_num) + ": write failed");
    }
  } else {
    Serial.println("Reader " + String(reader_num) + ":  tag not exist, cant write");
  }
  // delay(10);
}




