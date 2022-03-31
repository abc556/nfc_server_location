#include <string>
#include <vector>
#include <array>
#include <iostream>
#include <ArduinoJSON.h>
#include "nfc_pn532.h"
#include "wifi_c.h"
#include "http.h"
#include "mqtt.h"

// rack configurations //
#define U_INTERVAL 2
#define STARTING_U 16
#define ALERT_THRESHOLD 4
#define HTTP_USERNAME "admin"
#define HTTP_PASSWORD "Monit@r#1"
///////////////////////////

#define OPERATIONAL 1
#define RESERVED_AVAILABLE 5
#define RESERVED_MOVE 6
#define PLAN_DEMMISSION 9
#define RESERVED_PROCUREMENT 4
#define ALERT_TOTAL_COUNT 9
#define ALERT_ID_ADD 1
#define ALERT_ID_INCORRECT 2
#define ALERT_ID_MISSING 3
#define ALERT_ID_DECOMMISSION 4
#define ALERT_ID_NORMAL 5
#define ALERT_ID_UNKNOWN 6
#define ALERT_ID_MISSING_TAG 7
#define ALERT_ID_ALREADY_WRITTEN 8
#define ALERT_ID_UPDATED 9
#define STATUS_ID_NORMAL "N0"
#define STATUS_ID_WARNING "W1"
#define STATUS_ID_CRITICAL "C2"

extern int count_door_closed;

extern int reader_instance_idx [READER_COUNT];
extern boolean alert_idx_status [ALERT_TOTAL_COUNT];

extern boolean door_closed_n_fetched_rack_info;
extern boolean door_closed_n_mqtt_published;
extern boolean is_critical;

extern int mqtt_pub_count;

extern String mqtt_json_string;

void readerInstanceIdxInit();

void falsifyAlertIsTriggered();

boolean doorIsOpened();

void fetchRackName();
void fetchRackStatusJson();
void updateInstanceReaderMapping();

void postAssetTag(String id, String model_id, String name, String description, String nfc_id);

String updateReaderStatus(NfcAdapter &nfc, int reader_num);
void updateAllReadersStatus();

boolean instanceAlertPublish(std::vector<int> &array, String status_id, int alert_id, String mqtt_content_note);
boolean readerAlertPublish(std::vector<int> &array, String status_id, int alert_id, String mqtt_content_note);
void normalStatusPublish(String status_id, int alert_id);
void alertsAccordingToReaderStatuses();