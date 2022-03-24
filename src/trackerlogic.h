#include <string>
#include <vector>
#include <array>
#include <iostream>
#include "nfc_pn532.h"
#include "wifi_c.h"
#include "http.h"
#include "mqtt.h"


extern int u_interval;

void updateRackInfoJson();

void updateSupposedReadings(int start_u);

void rackInfoAlertChecking();

void postServerAssetTag(String id, String uid);

void updateReaderStatus(NfcAdapter &nfc, int reader_num);



