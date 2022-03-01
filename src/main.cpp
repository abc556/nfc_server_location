#include "nfc_pn532.h"
#include "wifi_c.h"

u_int p_time = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("Serial Begin.");

  //nfc begin
  nfc_begin();
  Serial.println("NFC Begin.");

  //wifi init
  // wifi_init();
  // Serial.println("Wifi Init.");

  //mqtt init

}

void loop() {
  // if(millis() - p_time >= 1000) {
  //   p_time = millis();
  //   Serial.println("-----------------------");
  //   // Serial.println(millis());
  // }

  //nfc
  nfc_loop();
}
