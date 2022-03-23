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
int reader_correct_idx [5] {-1,-1,-1,-1,-1};

const int ALERT_THRESHOLD = 3;

int no_nfc_count = 0;
int nfc1_count_missing = 0;
int nfc1_count_empty = 0;
int nfc1_count_incorrect = 0;
int nfc1_count_redundant = 0;
int nfc2_count_missing = 0;
int nfc2_count_empty = 0;
int nfc2_count_incorrect = 0;
int nfc2_count_redundant = 0;
int nfc3_count_missing = 0;
int nfc3_count_empty = 0;
int nfc3_count_incorrect = 0;
int nfc3_count_redundant = 0;
int nfc4_count_missing = 0;
int nfc4_count_empty = 0;
int nfc4_count_incorrect = 0;
int nfc4_count_redundant = 0;
int nfc5_count_missing = 0;
int nfc5_count_empty = 0;
int nfc5_count_incorrect = 0;
int nfc5_count_redundant = 0;

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
    for(int i=0; i<5; i++) {
        print_string += reader_correct_idx[i];
        print_string += " ";
    }
    Serial.println(print_string);
}

void rackInfoAlertChecking(){
    int reader_no = 0;
    int nfc_reader_idx = 0;
    int supposed_instance_idx = 0;
    std::vector<int> alert_empty_nfc_array;
    std::vector<int> alert_incorrect_instance_array;
    std::vector<int> alert_missing_instance_array;
    std::vector<int> alert_decommission_instance_array;
    std::vector<int> alert_redundant_nfc_array;
    String reader_result = "";

    no_nfc_count = 0;
    
    // nfc reader 1
    reader_result = nfc1_read();
    reader_no = 1;
    nfc_reader_idx = reader_no - 1;
    supposed_instance_idx = reader_correct_idx[nfc_reader_idx];
    if( supposed_instance_idx != -1) { // supposed to have nfc/server being detected
        if (reader_result == "NO_NFC") { // !nfc.tagPresent(...)
            no_nfc_count++;
            if (nfc1_count_missing >= ALERT_THRESHOLD) {
                // Serial.println("Reader " + (nfc_reader_idx+1).c_str() + ": no nfc sticker");
                Serial.println("Reader 1: no nfc sticker");
                alert_missing_instance_array.push_back(supposed_instance_idx);
            } else {
                Serial.println("Reader 1: detecting...");
                nfc1_count_missing++;
            }
        } else {
            // Serial.println("Reader " + (nfc_reader_idx+1).c_str() + ": sticker exists");
            Serial.println("Reader 1: sticker exists");
            nfc1_count_missing = 0;
            String instance_id = rack_json["data"][supposed_instance_idx]["id"];
            if (reader_result.length<5) {

            }
            if (reader_result.indexOf(instance_id) == -1) { // instance/server id does not exist(match) in nfc sticker
                if (nfc1_count_incorrect >= ALERT_THRESHOLD) {
                    // Serial.println("Reader " + (nfc_reader_idx+1).c_str() + ": incorrect sticker contetnt (server id)");
                    Serial.println("Reader 1: incorrect sticker contetnt (server id)");
                    nfc1_write(instance_id);
                    alert_incorrect_instance_array.push_back(supposed_instance_idx);
                    postServerAssetTag(instance_id, nfc1_getuid());
                    Serial.println("Posted nfc1");
                } else {
                    nfc1_count_incorrect++;
                }
            } else {
                Serial.println("Reader 1: sticker correct");
                nfc1_count_incorrect = 0;
            }
        }
    } else { // not supposed to have nfc/server being detected
        if (reader_result != "NO_NFC") {
            if (nfc1_count_redundant >= ALERT_THRESHOLD) {
                Serial.println("Reader 1: redundant sticker");
                alert_redundant_nfc_array.push_back(nfc_reader_idx);
            } else {
                nfc1_count_redundant++;
            }
        } else {
            no_nfc_count++;
            nfc1_count_redundant = 0;
        }
    }
    // nfc reader 2
    reader_result = nfc2_read();
    reader_no = 2;
    nfc_reader_idx = reader_no - 1;
    supposed_instance_idx = reader_correct_idx[nfc_reader_idx];
    if (supposed_instance_idx != -1) { // supposed to have nfc/server being detected
        if (reader_result == "NO_NFC") { // !nfc.tagPresent(...)
            no_nfc_count++;
            if (nfc2_count_missing >= ALERT_THRESHOLD) {
                if (rack_json["data"][supposed_instance_idx]["status"] == PLAN_DEMMISSION) {
                    Serial.println("Reader 2: ALERT(4) - decommissioned");
                    alert_decommission_instance_array.push_back(supposed_instance_idx);
                } else {
                    Serial.println("Reader 2: ALERT(3) - missing nfc tag");
                    alert_missing_instance_array.push_back(supposed_instance_idx);
                }
            } else {
                Serial.println("Reader 2: detecting...");
                nfc2_count_missing++;
            }
        } else { // nfc sticker read
            nfc2_count_missing = 0;
            String instance_id = rack_json["data"][supposed_instance_idx]["id"];
            if (reader_result.length <= 5) {
                nfc2_count_incorrect = 0;
                if (nfc2_count_empty >= ALERT_THRESHOLD) {
                    Serial.println("Reader 2: INFO(1) - added/wrote nfc content")
                    alert_empty_nfc_array.push_back(supposed_instance_idx);
                    nfc2_write(instance_id);
                    postServerAssetTag(instance_id, nfc2_getuid());
                } else {
                    nfc2_count_empty++;
                }
            } else if (reader_result.indexOf(instance_id) == -1) { // instance/server id does not exist(match) in nfc sticker
                nfc2_count_empty = 0;
                if (nfc2_count_incorrect >= ALERT_THRESHOLD) {
                    Serial.println("Reader 2: ALERT(2） - unmatched nfc content");
                    alert_incorrect_instance_array.push_back(supposed_instance_idx);
                } else {
                    nfc2_count_incorrect++;
                }
            } else {
                Serial.println("Reader 2: INFO(5) - normal (correct)");
                nfc2_count_empty = 0;
                nfc2_count_incorrect = 0;
            }
        }
    } else { // not supposed to have nfc/server being detected
        if (reader_result != "NO_NFC") {
            if (nfc2_count_redundant >= ALERT_THRESHOLD) {
                Serial.println("Reader 2: ALERT(6) - unknown nfc tag detected");
                alert_redundant_nfc_array.push_back(nfc_reader_idx);
            } else {
                nfc2_count_redundant++;
            }
        } else {
            no_nfc_count++;
            nfc2_count_redundant = 0;
        }
    }
    // nfc reader 3
    reader_result = nfc3_read();
    reader_no = 3;
    nfc_reader_idx = reader_no - 1;
    supposed_instance_idx = reader_correct_idx[nfc_reader_idx];
    if( supposed_instance_idx != -1) { // supposed to have nfc/server being detected
        if (reader_result == "NO_NFC") { // !nfc.tagPresent(...)
            no_nfc_count++;
            if (nfc3_count_missing >= ALERT_THRESHOLD) {
                Serial.println("Reader 3: no nfc sticker");
                alert_missing_instance_array.push_back(supposed_instance_idx);
            } else {
                Serial.println("Reader 3: detecting...");
                nfc3_count_missing++;
            }
        } else {
            Serial.println("Reader 3: sticker exists");
            nfc3_count_missing = 0;
            String instance_id = rack_json["data"][supposed_instance_idx]["id"];
            if (reader_result.indexOf(instance_id) == -1) { // instance/server id does not exist(match) in nfc sticker
                if (nfc3_count_incorrect >= ALERT_THRESHOLD) {
                    Serial.println("Reader 3: incorrect sticker contetnt (server id)");
                    nfc3_write(instance_id);
                    alert_incorrect_instance_array.push_back(supposed_instance_idx);
                    postServerAssetTag(instance_id, nfc3_getuid());
                    Serial.println("Post nfc3");
                } else {
                    nfc3_count_incorrect++;
                }
            } else {
                Serial.println("Reader 3: sticker correct");
                nfc3_count_incorrect = 0;
            }
        }
    } else { // not supposed to have nfc/server being detected
        if (reader_result != "NO_NFC") {
            no_nfc_count++;
            if (nfc3_count_redundant >= ALERT_THRESHOLD) {
                Serial.println("Reader 3: redundant sticker");
                alert_redundant_nfc_array.push_back(nfc_reader_idx);
            } else {
                nfc3_count_redundant++;
            }
        } else {
            nfc3_count_redundant = 0;
        }
    }
    // nfc reader 4
    reader_result = nfc4_read();
    reader_no = 4;
    nfc_reader_idx = reader_no - 1;
    supposed_instance_idx = reader_correct_idx[nfc_reader_idx];
    if( supposed_instance_idx != -1) { // supposed to have nfc/server being detected
        if (reader_result == "NO_NFC") { // !nfc.tagPresent(...)
            no_nfc_count++;
            if (nfc4_count_missing >= ALERT_THRESHOLD) {
                Serial.println("Reader 4: no nfc sticker");
                alert_missing_instance_array.push_back(supposed_instance_idx);
            } else {
                Serial.println("Reader 4: detecting...");
                nfc4_count_missing++;
            }
        } else {
            Serial.println("Reader 4: sticker exists");
            nfc4_count_missing = 0;
            String instance_id = rack_json["data"][supposed_instance_idx]["id"];
            if (reader_result.indexOf(instance_id) == -1) { // instance/server id does not exist(match) in nfc sticker
                if (nfc4_count_incorrect >= ALERT_THRESHOLD) {
                    Serial.println("Reader 4: incorrect sticker contetnt (server id)");
                    nfc4_write(instance_id);
                    alert_incorrect_instance_array.push_back(supposed_instance_idx);
                    postServerAssetTag(instance_id, nfc4_getuid());
                    Serial.println("Post nfc4");
                } else {
                    nfc4_count_incorrect++;
                }
            } else {
                Serial.println("Reader 4: sticker correct");
                nfc4_count_incorrect = 0;
            }
        }
    } else { // not supposed to have nfc/server being detected
        if (reader_result != "NO_NFC") {
            if (nfc4_count_redundant >= ALERT_THRESHOLD) {
                Serial.println("Reader 4: redundant sticker");
                alert_redundant_nfc_array.push_back(nfc_reader_idx);
            } else {
                nfc4_count_redundant++;
            }
        } else {
            no_nfc_count++;
            nfc4_count_redundant = 0;
        }
    }
    // nfc reader 5
    reader_result = nfc5_read();
    reader_no = 5;
    nfc_reader_idx = reader_no - 1;
    supposed_instance_idx = reader_correct_idx[nfc_reader_idx];
    if( supposed_instance_idx != -1) { // supposed to have nfc/server being detected
        if (reader_result == "NO_NFC") { // !nfc.tagPresent(...)
            no_nfc_count++;
            if (nfc5_count_missing >= ALERT_THRESHOLD) {
                Serial.println("Reader 5: no nfc sticker");
                alert_missing_instance_array.push_back(supposed_instance_idx);
            } else {
                Serial.println("Reader 5: detecting...");
                nfc5_count_missing++;
            }
        } else {
            Serial.println("Reader 5: sticker exists");
            nfc5_count_missing = 0;
            String instance_id = rack_json["data"][supposed_instance_idx]["id"];
            if (reader_result.indexOf(instance_id) == -1) { // instance/server id does not exist(match) in nfc sticker
                if (nfc5_count_incorrect >= ALERT_THRESHOLD) {
                    Serial.println("Reader 5: incorrect sticker contetnt (server id)");
                    nfc5_write(instance_id);
                    alert_incorrect_instance_array.push_back(supposed_instance_idx);
                    postServerAssetTag(instance_id, nfc5_getuid());
                    Serial.println("Post nfc5");
                } else {
                    nfc5_count_incorrect++;
                }
            } else {
                Serial.println("Reader 5: sticker correct");
                nfc5_count_incorrect = 0;
            }
        }
    } else { // not supposed to have nfc/server being detected
        if (reader_result != "NO_NFC") {
            if (nfc5_count_redundant >= ALERT_THRESHOLD) {
                Serial.println("Reader 5: redundant sticker");
                alert_redundant_nfc_array.push_back(nfc_reader_idx);
            } else {
                nfc5_count_redundant++;
            }
        } else {
            no_nfc_count++;
            nfc5_count_redundant = 0;
        }
    }
    
    
    // alerts mqtt publish
    DynamicJsonDocument alert_json(1024);
    String alert_string = "";
    String content = "";
    boolean normal_condition = true;
    if (no_nfc_count < READER_COUNT) {
        if (alert_empty_nfc_array.size() > 0) {
            normal_condition = false;
            int arr_size = alert_empty_nfc_array.size();
            int arr[arr_size];
            std::copy(alert_empty_nfc_array.begin(), alert_empty_nfc_array.end(), arr);
            alert_json["id"] = ALERT_ID_ADD;
            content = "Added new asset tag for devices: ";
            for (int i = 0; i < arr_size; i++) {
                content += "\"" + rack_json["data"][arr[i]]["name"] + "\"";
                if (i < arr_size - 1) { content += ", "; }
            }
            alert_json["content"] = content;
            serializeJson(alert_json, alert_string);
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
                content += "\"" + rack_json["data"][arr[i]]["name"] + "\"";
                if (i < arr_size - 1) { content += ", "; }
            }
            alert_json["content"] = content;
            serializeJson(alert_json, alert_string);
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
                content += "\"" + rack_json["data"][arr[i]]["name"] + "\"";
                if (i < arr_size - 1) { content += ", "; }
            }
            alert_json["content"] = content;
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
            content = "Decommissioned devices: "
            for (int i = 0; i < arr_size; i++) {
                content += "\"" + rack_json["data"][arr[i]]["name"] + "\"";
                if (i < arr_size - 1) { content += ", "; }
            }
            alert_json["content"] = content;
            serializeJson(alert_json, alert_string);
            Serial.println("Decommission: " + alert_string);
            mqtt_pub(alert_string);
        }
        if (alert_redundant_nfc_array.size() > 0) {
            normal_condition = false;
            int arr_size = alert_redundant_nfc_array.size();
            int arr[arr_size];
            std::copy(alert_redundant_nfc_array.begin(), alert_redundant_nfc_array.end(), arr);
            alert_json["id"] = ALERT_ID_UNKNOWN;
            content = "Uknown asset tag at readers: "
            for (int i = 0; i < arr_size; i++) {
                content += String(arr[i]+1);
                if (i < arr_size - 1) { content += ", "; }
            }
            alert_json["content"] = content;
            serializeJson(alert_json, alert_string);
            Serial.println("Unknown: " + alert_string);
            mqtt_pub(alert_string);
        }
        if (normal_condition) {
            alert_json["id"] = ALERT_ID_NORMAL;
            alert_json["content"] = "";
            alert_string = "";
            serializeJson(alert_json, alert_string);
            Serial.println("Normal condition")
            mqtt_pub(alert_string);
        }
    } else {
        Serial.println("-------------- Door opened --------------------");
    }
}

void postServerAssetTag(String id, String nfc_uid) {
    DynamicJsonDocument attribute_json(1024);
    attribute_json["id"] = "0aeddd86-ce3c-11dd-8da6-001d091dd9dd";
    attribute_json["name"] = "asset_tag";
    attribute_json["value"] = nfc_uid;
    DynamicJsonDocument post_json(1024);
    post_json["id"] = id;
    post_json["model_id"] = "5f50592c-44e2-11de-9ac7-000d566af2f2";
    post_json["name"] = "NFC server demo";
    post_json["description"] = "";
    post_json["attributes"][0] = attribute_json;
    String post_serial = "";
    serializeJson(post_json, post_serial);
    Serial.println(post_serial);
    http_post("/devices", post_serial, true, http_username, http_password);
}