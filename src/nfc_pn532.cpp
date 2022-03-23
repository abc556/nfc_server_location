#include "nfc_pn532.h"
#include "mqtt.h"

const int reader_count = 2;

PN532_SPI pn532spi1(SPI, CS1);
PN532_SPI pn532spi2(SPI, CS2);
// PN532_SPI pn532spi3(SPI, CS3);
// PN532_SPI pn532spi4(SPI, CS4);
// PN532_SPI pn532spi5(SPI, CS5);
NfcAdapter nfc1 = NfcAdapter(pn532spi1);
NfcAdapter nfc2 = NfcAdapter(pn532spi2);
// NfcAdapter nfc3 = NfcAdapter(pn532spi3);
// NfcAdapter nfc4 = NfcAdapter(pn532spi4);
// NfcAdapter nfc5 = NfcAdapter(pn532spi5);

String mqtt_data = "";

String nfc_data1 = "0";
String nfc_data2 = "0";
// String nfc_data3 = "0";
// String nfc_data4 = "0";

String nfc1_uid = "";
String nfc2_uid = "";
String nfc3_uid = "";
String nfc4_uid = "";
String nfc5_uid = "";
String nfc6_uid = "";

void nfc_begin(){
    nfc1.begin();
    nfc2.begin();
    // nfc3.begin();
    // nfc4.begin();
    // nfc5.begin();
}

//R/W 讀卡器模式
void nfc_loop(){
    mqtt_data = "";
    //nfc1
    if (nfc1.tagPresent(NFC_TIMEOUT))
    {
      // read
      NfcTag tag = nfc1.read();
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
          nfc_data1 = "";
          for (int c = 0; c < payloadLength; c++) {
            nfc_data1 += (char)payload[c];
          }
          // Serial.print("  Payload (as String): ");
          Serial.println(nfc_data1);

          //MQTT 1
          //mqtt_data += "\"1\":" + payloadAsString + "\",";
        }
      }

      //write
      NdefMessage message = NdefMessage();
      message.addTextRecord("Hello, Arduino!");
      if(nfc1.write(message)){
        Serial.println("Write success.");
      }else{
        Serial.println("Write failed.");
      }
      
    }else {
      nfc_data1 = "\"\"";
      nfc1.begin();
      // Serial.println("NFC1 Search Again...");
    }

    //nfc2
    if (nfc2.tagPresent(NFC_TIMEOUT))
    {
      NfcTag tag = nfc2.read();
      Serial.println("NFC2:");

      if (tag.hasNdefMessage()) // every tag won't have a message
      {
        NdefMessage message = tag.getNdefMessage();

        int recordCount = message.getRecordCount();
        for (int i = 0; i < recordCount; i++)
        {
          NdefRecord record = message.getRecord(i);

          int payloadLength = record.getPayloadLength();
          byte payload[payloadLength];
          record.getPayload(payload);

          nfc_data2 = "";
          for (int c = 0; c < payloadLength; c++) {
            nfc_data2 += (char)payload[c];
          }
          Serial.println(nfc_data2);
        }
      }
    }else {
      nfc_data2 = "\"\"";
      nfc2.begin();
      // Serial.println("NFC2 Search Again...");
    }

    mqtt_data = "{\"1\":" + nfc_data1 + 
                ",\"2\":" + "\"\""
                ",\"3\":" + "\"\""
                ",\"4\":" + "\"\""
                ",\"5\":" + nfc_data2 + 
                ",\"6\":" + "\"\""
                "}";
    mqtt_pub(mqtt_data);
}

void nfc_read(){
  String* nfc_readings = new String[12];
  if (nfc1.tagPresent(NFC_TIMEOUT)) {
    NfcTag tag = nfc1.read();
    if (tag.hasNdefMessage()) {
      NdefMessage message = tag.getNdefMessage();
      int recordCount = message.getRecordCount();
      for (int i = 0; i < recordCount; i++) {
        NdefRecord record = message.getRecord(i);
        int payloadLength = record.getPayloadLength();
        byte payload[payloadLength];
        record.getPayload(payload);
        nfc_data1 = "";
        for (int c = 0; c < payloadLength; c++) {
          nfc_data1 += (char)payload[c];
        }
        nfc_readings[0] = nfc_data1;
      }
    }
  } else {
    nfc_readings[0] = "nonfc";
  }
  if (nfc2.tagPresent(NFC_TIMEOUT)) {
    NfcTag tag = nfc2.read();
    if (tag.hasNdefMessage()) {
      NdefMessage message = tag.getNdefMessage();
      int recordCount = message.getRecordCount();
      for (int i = 0; i < recordCount; i++) {
        NdefRecord record = message.getRecord(i);
        int payloadLength = record.getPayloadLength();
        byte payload[payloadLength];
        record.getPayload(payload);
        nfc_data2 = "";
        for (int c = 0; c < payloadLength; c++) {
          nfc_data2 += (char)payload[c];
        }
        nfc_readings[1] = nfc_data2;
      }
    }
  } else {
    nfc_readings[1] = "nonfc";
  }
}

String nfc1_read(){
  String nfc_data = "";
  if (nfc1.tagPresent(NFC_TIMEOUT)) {
    NfcTag tag = nfc1.read();
    if (tag.hasNdefMessage()) {
      nfc1_uid = tag.getUidString();
      NdefMessage message = tag.getNdefMessage();
      int recordCount = message.getRecordCount();
      for (int i = 0; i < recordCount; i++) {
        NdefRecord record = message.getRecord(i);
        int payloadLength = record.getPayloadLength();
        byte payload[payloadLength];
        record.getPayload(payload);
        for (int c = 0; c < payloadLength; c++) {
          nfc_data += (char)payload[c];
        }
        return nfc_data;
      }
    }
  } else {
    return "NO_NFC";
  }
}

String nfc2_read(){
  String nfc_data = "";
  if (nfc2.tagPresent(NFC_TIMEOUT)) {
    NfcTag tag = nfc2.read();
    if (tag.hasNdefMessage()) {
      nfc2_uid = tag.getUidString();
      NdefMessage message = tag.getNdefMessage();
      int recordCount = message.getRecordCount();
      for (int i = 0; i < recordCount; i++) {
        NdefRecord record = message.getRecord(i);
        int payloadLength = record.getPayloadLength();
        byte payload[payloadLength];
        record.getPayload(payload);
        for (int c = 0; c < payloadLength; c++) {
          nfc_data += (char)payload[c];
        }
        return nfc_data;
      }
    }
  } else {
    return "NO_NFC";
  }
}

// String nfc3_read(){
//   String nfc_data = "";
//   if (nfc3.tagPresent(NFC_TIMEOUT)) {
//     NfcTag tag = nfc3.read();
//     if (tag.hasNdefMessage()) {
//       nfc3_uid = tag.getUidString();
//       NdefMessage message = tag.getNdefMessage();
//       int recordCount = message.getRecordCount();
//       for (int i = 0; i < recordCount; i++) {
//         NdefRecord record = message.getRecord(i);
//         int payloadLength = record.getPayloadLength();
//         byte payload[payloadLength];
//         record.getPayload(payload);
//         for (int c = 0; c < payloadLength; c++) {
//           nfc_data += (char)payload[c];
//         }
//         return nfc_data;
//       }
//     }
//   } else {
//     return "NO_NFC";
//   }
// }

// String nfc4_read(){
//   String nfc_data = "";
//   if (nfc4.tagPresent(NFC_TIMEOUT)) {
//     NfcTag tag = nfc4.read();
//     if (tag.hasNdefMessage()) {
//       nfc4_uid = tag.getUidString();
//       NdefMessage message = tag.getNdefMessage();
//       int recordCount = message.getRecordCount();
//       for (int i = 0; i < recordCount; i++) {
//         NdefRecord record = message.getRecord(i);
//         int payloadLength = record.getPayloadLength();
//         byte payload[payloadLength];
//         record.getPayload(payload);
//         for (int c = 0; c < payloadLength; c++) {
//           nfc_data += (char)payload[c];
//         }
//         return nfc_data;
//       }
//     }
//   } else {
//     return "NO_NFC";
//   }
// }

// String nfc5_read(){
//   String nfc_data = "";
//   if (nfc5.tagPresent(NFC_TIMEOUT)) {
//     NfcTag tag = nfc5.read();
//     if (tag.hasNdefMessage()) {
//       nfc5_uid = tag.getUidString();
//       NdefMessage message = tag.getNdefMessage();
//       int recordCount = message.getRecordCount();
//       for (int i = 0; i < recordCount; i++) {
//         NdefRecord record = message.getRecord(i);
//         int payloadLength = record.getPayloadLength();
//         byte payload[payloadLength];
//         record.getPayload(payload);
//         for (int c = 0; c < payloadLength; c++) {
//           nfc_data += (char)payload[c];
//         }
//         return nfc_data;
//       }
//     }
//   } else {
//     return "NO_NFC";
//   }
// }

String nfc1_getuid(){ return nfc1_uid; }
String nfc2_getuid(){ return nfc2_uid; }
String nfc3_getuid(){ return nfc3_uid; }
String nfc4_getuid(){ return nfc4_uid; }
String nfc5_getuid(){ return nfc5_uid; }

void nfc1_write(String content) {
  if (nfc1.tagPresent(NFC_TIMEOUT)) {
    NdefMessage message = NdefMessage();
    message.addTextRecord(content);
    if(nfc1.write(message)){
      Serial.println("Reader 1: write success");
    }else{
      Serial.println("Reader 1: write failed");
    }
  } else {
    Serial.println("NFC 1:  tag not exist, cant write");
  }
}

void nfc2_write(String content) {
  if (nfc2.tagPresent(NFC_TIMEOUT)) {
    NdefMessage message = NdefMessage();
    message.addTextRecord(content);
    if(nfc2.write(message)){
      Serial.println("Reader 2: write success");
    }else{
      Serial.println("Reader 2: write failed");
    }
  } else {
    Serial.println("NFC 2: tag not exist, cant write");
  }
}

// void nfc3_write(String content) {
//   if (nfc3.tagPresent(NFC_TIMEOUT)) {
//     NdefMessage message = NdefMessage();
//     message.addTextRecord(content);
//     if(nfc3.write(message)){
//       Serial.println("Reader 3: write success");
//     }else{
//       Serial.println("Reader 3: write failed");
//     }
//   } else {
//     Serial.println("NFC 3: tag not exist, cant write");
//   }
// }

// void nfc4_write(String content) {
//   if (nfc4.tagPresent(NFC_TIMEOUT)) {
//     NdefMessage message = NdefMessage();
//     message.addTextRecord(content);
//     if(nfc4.write(message)){
//       Serial.println("Reader 4: write success");
//     }else{
//       Serial.println("Reader 4: write failed");
//     }
//   } else {
//     Serial.println("NFC 4: tag not exist, cant write");
//   }
// }

// void nfc5_write(String content) {
//   if (nfc5.tagPresent(NFC_TIMEOUT)) {
//     NdefMessage message = NdefMessage();
//     message.addTextRecord(content);
//     if(nfc5.write(message)){
//       Serial.println("Reader 5: write success");
//     }else{
//       Serial.println("Reader 5: write failed");
//     }
//   } else {
//     Serial.println("NFC 5: tag not exist, cant write");
//   }
// }
