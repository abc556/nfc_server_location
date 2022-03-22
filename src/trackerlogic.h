#include "nfc_pn532.h"
#include "wifi_c.h"
#include "http.h"

// extern DynamicJsonDocument rack_json(1024);
extern int reader_correct_idx [2];
extern int min_U;

void updateRackInfoJson();

void updateSupposedReadings(int start_u);

void rackInfoAlertChecking();



