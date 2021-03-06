#include "nfc_pn532.h"
#include "mqtt.h"

PN532_SPI pn532spi1(SPI, CS1);
PN532_SPI pn532spi2(SPI, CS2);
// PN532_SPI pn532spi3(SPI, CS3);
// PN532_SPI pn532spi4(SPI, CS4);
NfcAdapter nfc1 = NfcAdapter(pn532spi1);
NfcAdapter nfc2 = NfcAdapter(pn532spi2);
// NfcAdapter nfc3 = NfcAdapter(pn532spi3);
// NfcAdapter nfc4 = NfcAdapter(pn532spi4);

String mqtt_data = "";

String nfc_data1 = "0";
String nfc_data2 = "0";
// String nfc_data3 = "0";
// String nfc_data4 = "0";

void nfc_begin(){
    nfc1.begin();
    nfc2.begin();
    // nfc3.begin();
    // nfc4.begin();
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