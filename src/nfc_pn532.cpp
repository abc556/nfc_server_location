#include "nfc_pn532.h"

PN532_SPI pn532spi1(SPI, CS1);
PN532_SPI pn532spi2(SPI, CS2);
PN532_SPI pn532spi3(SPI, CS3);
PN532_SPI pn532spi4(SPI, CS4);
NfcAdapter nfc1 = NfcAdapter(pn532spi1);
NfcAdapter nfc2 = NfcAdapter(pn532spi2);
NfcAdapter nfc3 = NfcAdapter(pn532spi3);
NfcAdapter nfc4 = NfcAdapter(pn532spi4);

void nfc_begin(){
    nfc1.begin();
    nfc2.begin();
    nfc3.begin();
    nfc4.begin();
}

String nfc_read(NfcAdapter nfcAdapter){
  if (nfcAdapter.tagPresent(NFC_TIMEOUT))
    {
      NfcTag tag = nfcAdapter.read();
      Serial.println("NFC1:");
      // Serial.println(tag.getTagType());
      // Serial.print("UID: ");Serial.println(tag.getUidString());
        
      if (tag.hasNdefMessage()) // every tag won't have a message
      {
        NdefMessage message = tag.getNdefMessage();

        int recordCount = message.getRecordCount();
        for (int i = 0; i < recordCount; i++)
        {
          // Serial.print("\nNDEF Record ");Serial.println(i+1);
          NdefRecord record = message.getRecord(i);
          // NdefRecord record = message[i]; // alternate syntax

          //Serial.print("  TNF: ");Serial.println(record.getTnf());
          //Serial.print("  Type: ");Serial.println(record.getType()); // will be "" for TNF_EMPTY

          int payloadLength = record.getPayloadLength();
          byte payload[payloadLength];
          record.getPayload(payload);

          // // Print the Hex and Printable Characters
          // Serial.print("  Payload (HEX): ");
          // PrintHexChar(payload, payloadLength);

          // Force the data into a String (might work depending on the content)
          // Real code should use smarter processing
          String payloadAsString = "";
          for (int c = 0; c < payloadLength; c++) {
            payloadAsString += (char)payload[c];
          }
          // Serial.print("  Payload (as String): ");
          Serial.println(payloadAsString);

          return payloadAsString;
        }
      }
    } else {
      return "";
    }
}

String nfc_reading_combine(){
  String readings = "";
  readings += "\"1\":"+nfc_read(nfc1)+"\",";
  readings += "\"2\":"+nfc_read(nfc2)+"\",";
  readings += "\"3\":"+nfc_read(nfc3)+"\",";
  readings += "\"4\":"+nfc_read(nfc4)+"\",";
  return readings;
}

//R/W 讀卡器模式
void nfc_loop(){
    // if (nfc1.tagPresent(NFC_TIMEOUT))
    // {
    //   NfcTag tag = nfc1.read();
    //   Serial.println("NFC1:");
    //   // Serial.println(tag.getTagType());
    //   // Serial.print("UID: ");Serial.println(tag.getUidString());
        
    //   if (tag.hasNdefMessage()) // every tag won't have a message
    //   {
    //     NdefMessage message = tag.getNdefMessage();

    //     int recordCount = message.getRecordCount();
    //     for (int i = 0; i < recordCount; i++)
    //     {
    //       // Serial.print("\nNDEF Record ");Serial.println(i+1);
    //       NdefRecord record = message.getRecord(i);
    //       // NdefRecord record = message[i]; // alternate syntax

    //       //Serial.print("  TNF: ");Serial.println(record.getTnf());
    //       //Serial.print("  Type: ");Serial.println(record.getType()); // will be "" for TNF_EMPTY

    //       int payloadLength = record.getPayloadLength();
    //       byte payload[payloadLength];
    //       record.getPayload(payload);

    //       // // Print the Hex and Printable Characters
    //       // Serial.print("  Payload (HEX): ");
    //       // PrintHexChar(payload, payloadLength);

    //       // Force the data into a String (might work depending on the content)
    //       // Real code should use smarter processing
    //       String payloadAsString = "";
    //       for (int c = 0; c < payloadLength; c++) {
    //         payloadAsString += (char)payload[c];
    //       }
    //       // Serial.print("  Payload (as String): ");
    //       Serial.println(payloadAsString);

    //       //MQTT 1
    //     }
    //   }
    // }//nfc1
    // if (nfc2.tagPresent(NFC_TIMEOUT))
    // {
    //   NfcTag tag = nfc2.read();
    //   Serial.println("NFC2:");

    //   if (tag.hasNdefMessage()) // every tag won't have a message
    //   {
    //     NdefMessage message = tag.getNdefMessage();

    //     int recordCount = message.getRecordCount();
    //     for (int i = 0; i < recordCount; i++)
    //     {
    //       NdefRecord record = message.getRecord(i);

    //       int payloadLength = record.getPayloadLength();
    //       byte payload[payloadLength];
    //       record.getPayload(payload);

    //       String payloadAsString = "";
    //       for (int c = 0; c < payloadLength; c++) {
    //         payloadAsString += (char)payload[c];
    //       }
    //       Serial.println(payloadAsString);

    //       //MQTT 2
    //     }
    //   }
    // }//nfc2
}