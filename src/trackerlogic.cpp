#include "trackerlogic.h"

const String http_username = "admin";
const String http_password = "Monit@r#1";

const int OPERATIONAL = 1;
const int RESERVED_AVAILABLE = 5;
const int RESERVED_MOVE = 6;
const int PLAN_DEMMISSION = 9;

const int ALERT_ID_ADD = 1;
const int ALERT_ID_INCORRECT = 2;
const int ALERT_ID_MISSING = 3;
const int ALERT_ID_DECOMMISSION = 4;
const int ALERT_ID_NORMAL = 5;
const int ALERT_ID_UNKNOWN = 6;

int u_interval = 2;
DynamicJsonDocument rack_json(10240);
// int reader_correct_idx [2] {-1,-1};
// int reader_correct_idx [5] {-1,-1,-1,-1,-1};
int reader_correct_idx [10] {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};

const int ALERT_THRESHOLD = 4;
const int DOOR_CLOSED_REPORT_THRESHOLD = 5;

int no_nfc_count = 0;
int door_closed_count = 0;

int count_missing[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
int count_empty[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
int count_incorrect[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
int count_redundant[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };


std::vector<int> alert_empty_nfc_array;
std::vector<int> alert_incorrect_instance_array;
std::vector<int> alert_missing_instance_array;
std::vector<int> alert_decommission_instance_array;
std::vector<int> alert_redundant_nfc_array;

void updateRackInfoJson(){
    String content = http_get("/devices/rdinfo", "501ef728-6e4d-4d8d-b2b4-19ac01fcf96d", true, http_username, http_password);
    // String content = "{\"code\": \"1\",\"data\": [{\"description\": \"test\",\"mid\": \"5f50592c-44e2-11de-9ac7-000d566af2f2\",\"upos\": 40.0,\"pid\": \"ca7391ec-d002-11dd-8252-001d091dd9dd\",\"mname\": \"2950III\",\"acl\": 15,\"tid\": \"59e3262e-cd7b-11dd-81bf-001d091dd9dd\",\"sid\": 21115,\"vid\": \"6b16fb0e-cd81-11dd-83bf-001d091dd9dd\",\"is_reserved\": false,\"wo\": false,\"id\": \"cd4df3b8-2b9d-4aae-b047-85181022cefa\",\"sn\": null,\"eups\": null,\"owner\": null,\"side\": \"\",\"enclourse\": null,\"pname\": \"PowerEdge\",\"status_name\": \"Operational\",\"ip\": null,\"tname\": \"Server - Rackmount\",\"psid\": 1670,\"tsid\": 533,\"vsid\": 246,\"shelf\": null,\"msid\": 28436,\"slots\": null,\"vname\": \"Dell\",\"at\": \"at123456\",\"reserved\": false,\"name\": \"NFC server demo\",\"_id\": \"cd4df3b8-2b9d-4aae-b047-85181022cefa\",\"position\": \"40.0 U\",\"_pid_\": null,\"_is_child_\": false,\"status\": 1},{\"description\": null,\"mid\": \"9d2b566a-d52d-11ea-bf0d-b32c7d6d75cf\",\"upos\": 38.0,\"pid\": \"ca7391ec-d002-11dd-8252-001d091dd9dd\",\"mname\": \"R740xd (12D 3.5)\",\"acl\": 15,\"tid\": \"59e3262e-cd7b-11dd-81bf-001d091dd9dd\",\"sid\": 21116,\"vid\": \"6b16fb0e-cd81-11dd-83bf-001d091dd9dd\",\"is_reserved\": false,\"wo\": false,\"id\": \"032d6098-a43e-4d67-8def-2f2f5e823158\",\"sn\": null,\"eups\": null,\"owner\": null,\"side\": \"\",\"enclourse\": null,\"pname\": \"PowerEdge\",\"status_name\": \"Operatjoinional\",\"ip\": null,\"tname\": \"Server - Rackmount\",\"psid\": 1670,\"tsid\": 533,\"vsid\": 246,\"shelf\": null,\"msid\": 54715,\"slots\": null,\"vname\": \"Dell\",\"at\": null,\"reserved\": false,\"name\": \"NFC Server Demo -1\",\"_id\": \"032d6098-a43e-4d67-8def-2f2f5e823158\",\"position\": \"38.0 U\",\"_pid_\": null,\"_is_child_\": false,\"status\": 1}],\"message\": \"Success\"}";
    deserializeJson(rack_json, content);
}

void updateSupposedReadings(int start_u){
    int instance_count = 0;
    for (int i = 0; i<100; i++){
        String str = rack_json["data"][i];
        if(str!="null"){
            instance_count++;
        }else{
            break;
        }
    }
    Serial.println("Instance number: " + String(instance_count));

    for(int i = 0; i<instance_count; i++) {
        int u_position = rack_json["data"][i]["upos"]; // getting u position of an instance/server
        int reader_num = (u_position - start_u) / u_interval;  // calculate no. of reader which should detect that instance
        int status_id = rack_json["data"][i]["status"];
        if ( status_id == OPERATIONAL || status_id == RESERVED_AVAILABLE || status_id == RESERVED_MOVE || status_id == PLAN_DEMMISSION ) {
            reader_correct_idx [reader_num] = i; // eg.[1, 2, -1] 1&2: index of instance array, -1: supposed no nfc/instance should be detected
        }
    }
    String print_string = "reader - instance_index : ";
    for(int i=0; i < READER_COUNT; i++) {
        print_string += reader_correct_idx[i];
        print_string += " ";
    }
    Serial.println(print_string);
}

void rackInfoAlertChecking(){
    int reader_no = 0;
    int nfc_reader_idx = 0;
    int supposed_instance_idx = 0;
    alert_empty_nfc_array.clear();
    alert_incorrect_instance_array.clear();
    alert_missing_instance_array.clear();
    alert_decommission_instance_array.clear();
    alert_redundant_nfc_array.clear();
    no_nfc_count = 0;

    updateReaderStatus(nfc1, 1);
    updateReaderStatus(nfc2, 2);
    updateReaderStatus(nfc3, 3);
    updateReaderStatus(nfc4, 4);
    updateReaderStatus(nfc5, 5);
    // updateReaderStatus(nfc6, 6);
    // updateReaderStatus(nfc7, 7);
    // updateReaderStatus(nfc8, 8);
    // updateReaderStatus(nfc9, 9);
    // updateReaderStatus(nfc10, 10);

    
    // alerts mqtt publish
    DynamicJsonDocument alert_json(1024);
    String alert_string = "";
    String content = "";
    boolean normal_condition = true;
    Serial.println("no_nfc_count: " + String(no_nfc_count));
    Serial.println("doorclosecount: " + String(door_closed_count));
    if (no_nfc_count < READER_COUNT) {
        if (door_closed_count == DOOR_CLOSED_REPORT_THRESHOLD) {
            if (alert_empty_nfc_array.size() > 0) {
                normal_condition = false;
                int arr_size = alert_empty_nfc_array.size();
                int arr[arr_size];
                std::copy(alert_empty_nfc_array.begin(), alert_empty_nfc_array.end(), arr);
                alert_json["id"] = ALERT_ID_ADD;
                content = "Added new asset tag for devices: ";
                for (int i = 0; i < arr_size; i++) {
                    String name = rack_json["data"][arr[i]]["name"];
                    content += "[" + name + "]";
                    if (i < arr_size - 1) { content += ", "; }
                }
                alert_json["content"] = content;
                serializeJson(alert_json, alert_string);
                alert_string = "";
                Serial.println("Added: " + alert_string);
                mqtt_pub(alert_string);
            }
            if (alert_incorrect_instance_array.size() > 0) {
                normal_condition = false;
                int arr_size = alert_incorrect_instance_array.size();
                int arr[arr_size];
                std::copy(alert_incorrect_instance_array.begin(), alert_incorrect_instance_array.end(), arr);
                alert_json["id"] = ALERT_ID_INCORRECT;
                content = "Unmatched asset tag of devices: ";
                for (int i = 0; i < arr_size; i++) {
                    String name = rack_json["data"][arr[i]]["name"];
                    content += "[" + name + "]";
                    if (i < arr_size - 1) { content += ", "; }
                }
                alert_json["content"] = content;
                serializeJson(alert_json, alert_string);
                alert_string = "";
                Serial.println("Incorrect: " + alert_string);
                mqtt_pub(alert_string);
            }
            if (alert_missing_instance_array.size() > 0) {
                normal_condition = false;
                int arr_size = alert_missing_instance_array.size();
                int arr[arr_size];
                std::copy(alert_missing_instance_array.begin(), alert_missing_instance_array.end(), arr);
                alert_json["id"] = ALERT_ID_MISSING;
                content = "Missing asset tag of devices: ";
                for (int i = 0; i < arr_size; i++) {
                    String name = rack_json["data"][arr[i]]["name"];
                    content += "[" + name + "]";
                    if (i < arr_size - 1) { content += ", "; }
                }
                alert_json["content"] = content;
                alert_string = "";
                serializeJson(alert_json, alert_string);
                Serial.println("Missing: " + alert_string);
                mqtt_pub(alert_string);
            }
            if (alert_decommission_instance_array.size() > 0) {
                normal_condition = false;
                int arr_size = alert_missing_instance_array.size();
                int arr[arr_size];
                std::copy(alert_decommission_instance_array.begin(), alert_decommission_instance_array.end(), arr);
                alert_json["id"] = ALERT_ID_DECOMMISSION;
                content = "Decommissioned devices: ";
                for (int i = 0; i < arr_size; i++) {
                    String name = rack_json["data"][arr[i]]["name"];
                    content += "[" + name + "]";
                    if (i < arr_size - 1) { content += ", "; }
                }
                alert_json["content"] = content;
                serializeJson(alert_json, alert_string);
                alert_string = "";
                Serial.println("Decommission: " + alert_string);
                mqtt_pub(alert_string);
            }
            if (alert_redundant_nfc_array.size() > 0) {
                normal_condition = false;
                int arr_size = alert_redundant_nfc_array.size();
                alert_json["id"] = ALERT_ID_UNKNOWN;
                content = "Uknown asset tag at readers: ";
                for (int i = 0; i < arr_size; i++) {
                    String reader_id = String(alert_redundant_nfc_array[i]+1);
                    content += reader_id;
                    if (i < arr_size - 1) { content += ", "; }
                }
                alert_json["content"] = content;
                alert_string = "";
                serializeJson(alert_json, alert_string);
                Serial.println("Unknown: " + alert_string);
                mqtt_pub(alert_string);
            }
            if (normal_condition) {
                alert_json["id"] = ALERT_ID_NORMAL;
                alert_json["content"] = "";
                alert_string = "";
                serializeJson(alert_json, alert_string);
                Serial.println("Normal condition");
                mqtt_pub(alert_string);
            }
            door_closed_count = (DOOR_CLOSED_REPORT_THRESHOLD + 1);
        }else if (door_closed_count <= DOOR_CLOSED_REPORT_THRESHOLD) {
            door_closed_count++;
        }
    } else {
        Serial.println("-------------- Door opened --------------------");
        door_closed_count = 0;
    }
}

void postServerAssetTag(String id, String model_id, String name, String description, String nfc_uid) {
    DynamicJsonDocument attribute_json(1024);
    attribute_json["id"] = "0aeddd86-ce3c-11dd-8da6-001d091dd9dd";
    attribute_json["name"] = "asset_tag";
    attribute_json["value"] = nfc_uid;
    DynamicJsonDocument post_json(1024);
    post_json["id"] = id;
    post_json["model_id"] = model_id;
    post_json["name"] = name;
    post_json["description"] = description;
    post_json["attributes"][0] = attribute_json;
    String post_serial = "";
    serializeJson(post_json, post_serial);
    Serial.println("Post content: " + post_serial);
    http_post("/devices", post_serial, true, http_username, http_password);
}

void updateReaderStatus(NfcAdapter &nfc, int reader_num) {
    nfc_read(nfc, reader_num);
    int reader_idx = reader_num - 1;
    int supposed_instance_idx = reader_correct_idx[reader_idx];
    // Serial.println(nfc_content);
    if (supposed_instance_idx != -1) { // supposed to have nfc/server being detected
        Serial.println(nfc_content);
        if (nfc_content == "NO_NFC") { // !nfc.tagPresent(...)
            no_nfc_count++;
            if (count_missing[reader_idx] >= ALERT_THRESHOLD) {
                if (rack_json["data"][supposed_instance_idx]["status"] == PLAN_DEMMISSION) {
                    // Serial.println(" " + String(reader_num) + ": ALERT(4) - decommissioned");
                    alert_decommission_instance_array.push_back(supposed_instance_idx);
                } else {
                    // Serial.println(" " + String(reader_num) + ": ALERT(3) - missing nfc tag");
                    alert_missing_instance_array.push_back(supposed_instance_idx);
                }
            } else {
                // Serial.println(" " + String(reader_num) + ": detecting...");
                count_missing[reader_idx]++;
            }
        } else { // nfc sticker read
            count_missing[reader_idx] = 0;
            String instance_id = rack_json["data"][supposed_instance_idx]["id"];
            String model_id = rack_json["data"][supposed_instance_idx]["mid"];
            String name = rack_json["data"][supposed_instance_idx]["name"];
            String description = rack_json["data"][supposed_instance_idx]["description"];
            if (nfc_content.length() <= 5) {
                count_incorrect[reader_idx] = 0;
                if (count_empty[reader_idx] >= ALERT_THRESHOLD) {
                    Serial.println(" " + String(reader_num) + ": INFO(1) - added/wrote nfc content");
                    alert_empty_nfc_array.push_back(supposed_instance_idx);
                    nfc_write(nfc, reader_num, instance_id);
                    postServerAssetTag(instance_id, model_id, name, description, nfc_uid);
                    if (door_closed_count < DOOR_CLOSED_REPORT_THRESHOLD) {
                        door_closed_count == DOOR_CLOSED_REPORT_THRESHOLD;
                    }
                } else {
                    // Serial.println(" " + String(reader_num) + ": reading(empty)...");
                    count_empty[reader_idx]++;
                }
            } else if (nfc_content.indexOf(instance_id) == -1) { // instance/server id does not exist(match) in nfc sticker
                count_empty[reader_idx] = 0;
                if (count_incorrect[reader_idx] >= ALERT_THRESHOLD) {
                    Serial.println(" " + String(reader_num) + ": ALERT(2) - unmatched nfc content");
                    alert_incorrect_instance_array.push_back(supposed_instance_idx);
                    // nfc_write(nfc, reader_num, instance_id); /////////// temperary
                    // postServerAssetTag(instance_id, nfc_uid);  /////////// temperary
                    if (door_closed_count < DOOR_CLOSED_REPORT_THRESHOLD) {
                        door_closed_count == DOOR_CLOSED_REPORT_THRESHOLD;
                    }                
                } else {
                    // Serial.println(" " + String(reader_num) + ": reading(incorrect)...");
                    count_incorrect[reader_idx]++;
                }
            } else {
                // Serial.println(" " + String(reader_num) + ": INFO(5) - normal (correct)");
                count_empty[reader_idx] = 0;
                count_incorrect[reader_idx] = 0;
            }
        }
    } else { // not supposed to have nfc/server being detected
        if (nfc_content != "NO_NFC") {
            if (count_redundant[reader_idx] >= ALERT_THRESHOLD) {
                // Serial.println(" " + String(reader_num) + ": ALERT(6) - unknown nfc tag detected");
                alert_redundant_nfc_array.push_back(reader_idx);
            } else {
                    // Serial.println(" " + String(reader_num) + ": detecting (unknown)...");
                count_redundant[reader_idx]++;
            }
        } else {
            // Serial.println(" " + String(reader_num) + ": null (not supposed to have nfc)");
            no_nfc_count++;
            count_redundant[reader_idx] = 0;
        }
    }
}