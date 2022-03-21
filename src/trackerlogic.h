#include "nfc_pn532.h"
#include "wifi_c.h"
#include "http.h"

void getRackInfoJson();
void isNfcReadingCorrect(String DCIM_rackinfo, String* nfc_readings);
