#include "trackerlogic.h"

const String http_username = "admin";
const String http_password = "Monit@r#1";

int u_interval = 2;
DynamicJsonDocument rack_json(10240);
int reader_correct_idx [2] = { -1 };

const int ALERT_THRESHOLD = 3;

int nfc1_count_missing = 0;
int nfc1_count_incorrect = 0;
int nfc1_count_redundant = 0;
int nfc2_count_missing = 0;
int nfc2_count_incorrect = 0;
int nfc2_count_redundant = 0;
int nfc3_count_missing = 0;
int nfc3_count_incorrect = 0;
int nfc3_count_redundant = 0;
int nfc4_count_missing = 0;
int nfc4_count_incorrect = 0;
int nfc4_count_redundant = 0;
int nfc5_count_missing = 0;
int nfc5_count_incorrect = 0;
int nfc5_count_redundant = 0;

void updateRackInfoJson(){
    // String content = http_get("/devices/rdinfo", "501ef728-6e4d-4d8d-b2b4-19ac01fcf96d", true, http_username, http_password);
    String content = "{\"code\": \"1\",\"data\": [{\"description\": \"test\",\"mid\": \"5f50592c-44e2-11de-9ac7-000d566af2f2\",\"upos\": 40.0,\"pid\": \"ca7391ec-d002-11dd-8252-001d091dd9dd\",\"mname\": \"2950III\",\"acl\": 15,\"tid\": \"59e3262e-cd7b-11dd-81bf-001d091dd9dd\",\"sid\": 21115,\"vid\": \"6b16fb0e-cd81-11dd-83bf-001d091dd9dd\",\"is_reserved\": false,\"wo\": false,\"id\": \"cd4df3b8-2b9d-4aae-b047-85181022cefa\",\"sn\": null,\"eups\": null,\"owner\": null,\"side\": \"\",\"enclourse\": null,\"pname\": \"PowerEdge\",\"status_name\": \"Operational\",\"ip\": null,\"tname\": \"Server - Rackmount\",\"psid\": 1670,\"tsid\": 533,\"vsid\": 246,\"shelf\": null,\"msid\": 28436,\"slots\": null,\"vname\": \"Dell\",\"at\": \"at123456\",\"reserved\": false,\"name\": \"NFC server demo\",\"_id\": \"cd4df3b8-2b9d-4aae-b047-85181022cefa\",\"position\": \"40.0 U\",\"_pid_\": null,\"_is_child_\": false,\"status\": 1},{\"description\": null,\"mid\": \"9d2b566a-d52d-11ea-bf0d-b32c7d6d75cf\",\"upos\": 38.0,\"pid\": \"ca7391ec-d002-11dd-8252-001d091dd9dd\",\"mname\": \"R740xd (12D 3.5)\",\"acl\": 15,\"tid\": \"59e3262e-cd7b-11dd-81bf-001d091dd9dd\",\"sid\": 21116,\"vid\": \"6b16fb0e-cd81-11dd-83bf-001d091dd9dd\",\"is_reserved\": false,\"wo\": false,\"id\": \"032d6098-a43e-4d67-8def-2f2f5e823158\",\"sn\": null,\"eups\": null,\"owner\": null,\"side\": \"\",\"enclourse\": null,\"pname\": \"PowerEdge\",\"status_name\": \"Operatjoinional\",\"ip\": null,\"tname\": \"Server - Rackmount\",\"psid\": 1670,\"tsid\": 533,\"vsid\": 246,\"shelf\": null,\"msid\": 54715,\"slots\": null,\"vname\": \"Dell\",\"at\": null,\"reserved\": false,\"name\": \"NFC Server Demo -1\",\"_id\": \"032d6098-a43e-4d67-8def-2f2f5e823158\",\"position\": \"38.0 U\",\"_pid_\": null,\"_is_child_\": false,\"status\": 1}],\"message\": \"Success\"}";
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
    for(int i = 0; i<instance_count; i++) {
        int u_position = rack_json["data"][i]["upos"]; // getting u position of an instance/server
        int reader_num = (u_position - start_u) / u_interval;  // calculate no. of reader which should detect that instance
        reader_correct_idx [reader_num] = i; // eg.[1, 2, -1] 1&2: index of instance array, -1: supposed no nfc/instance should be detected
    }
}

void rackInfoAlertChecking(){
    int reader_no = 0;
    int nfc_reader_idx = 0;
    int supposed_instance_idx = 0;
    std::vector<int> alert_missing_instance_array;
    std::vector<int> alert_incorrect_instance_array;
    std::vector<int> alert_redundant_nfc_array;
    String reader_result = "";
    
    // nfc reader 1
    reader_result = nfc1_read();
    reader_no = 1;
    nfc_reader_idx = reader_no - 1;
    supposed_instance_idx = reader_correct_idx[nfc_reader_idx];
    if( supposed_instance_idx != -1) { // supposed to have nfc/server being detected
        if (reader_result == "NO_NFC") { // !nfc.tagPresent(...)
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
            nfc1_count_redundant = 0;
        }
    }
    // nfc reader 2
    reader_result = nfc2_read();
    reader_no = 2;
    nfc_reader_idx = reader_no - 1;
    supposed_instance_idx = reader_correct_idx[nfc_reader_idx];
    if( supposed_instance_idx != -1) { // supposed to have nfc/server being detected
        if (reader_result == "NO_NFC") { // !nfc.tagPresent(...)
            if (nfc2_count_missing >= ALERT_THRESHOLD) {
                Serial.println("Reader 2: no nfc sticker");
                alert_missing_instance_array.push_back(supposed_instance_idx);
            } else {
                Serial.println("Reader 2: detecting...");
                nfc2_count_missing++;
            }
        } else {
            Serial.println("Reader 2: sticker exists");
            nfc2_count_missing = 0;
            String instance_id = rack_json["data"][supposed_instance_idx]["id"];
            if (reader_result.indexOf(instance_id) == -1) { // instance/server id does not exist(match) in nfc sticker
                if (nfc2_count_incorrect >= ALERT_THRESHOLD) {
                    Serial.println("Reader 2: incorrect sticker contetnt (server id)");
                    nfc2_write(instance_id);
                    alert_incorrect_instance_array.push_back(supposed_instance_idx);
                    postServerAssetTag(instance_id, nfc1_getuid());
                    Serial.println("Post nfc2");
                } else {
                    nfc2_count_incorrect++;
                }
            } else {
                Serial.println("Reader 2: sticker correct");
                nfc2_count_incorrect = 0;
            }
        }
    } else { // not supposed to have nfc/server being detected
        if (reader_result != "NO_NFC") {
            if (nfc2_count_redundant >= ALERT_THRESHOLD) {
                Serial.println("Reader 2: redundant sticker");
                alert_redundant_nfc_array.push_back(nfc_reader_idx);
            } else {
                nfc2_count_redundant++;
            }
        } else {
            nfc2_count_redundant = 0;
        }
    }
    // // nfc reader 3
    // reader_result = nfc3_read();
    // reader_no = 3;
    // nfc_reader_idx = reader_no - 1;
    // supposed_instance_idx = reader_correct_idx[nfc_reader_idx];
    // if( supposed_instance_idx != -1) { // supposed to have nfc/server being detected
    //     if (reader_result == "NO_NFC") { // !nfc.tagPresent(...)
    //         if (nfc3_count_missing >= ALERT_THRESHOLD) {
    //             Serial.println("Reader 3: no nfc sticker");
    //             alert_missing_instance_array.push_back(supposed_instance_idx);
    //         } else {
    //             Serial.println("Reader 3: detecting...");
    //             nfc3_count_missing++;
    //         }
    //     } else {
    //         Serial.println("Reader 3: sticker exists");
    //         nfc3_count_missing = 0;
    //         String instance_id = rack_json["data"][supposed_instance_idx]["id"];
    //         if (reader_result.indexOf(instance_id) == -1) { // instance/server id does not exist(match) in nfc sticker
    //             if (nfc3_count_incorrect >= ALERT_THRESHOLD) {
    //                 Serial.println("Reader 3: incorrect sticker contetnt (server id)");
    //                 nfc3_write(instance_id);
    //                 alert_incorrect_instance_array.push_back(supposed_instance_idx);
    //                 // postServerAssetTag(instance_id, nfc1_getuid());
    //                 Serial.println("Post nfc3");
    //             } else {
    //                 nfc3_count_incorrect++;
    //             }
    //         } else {
    //             Serial.println("Reader 3: sticker correct");
    //             nfc3_count_incorrect = 0;
    //         }
    //     }
    // } else { // not supposed to have nfc/server being detected
    //     if (reader_result != "NO_NFC") {
    //         if (nfc3_count_redundant >= ALERT_THRESHOLD) {
    //             Serial.println("Reader 3: redundant sticker");
    //             alert_redundant_nfc_array.push_back(nfc_reader_idx);
    //         } else {
    //             nfc3_count_redundant++;
    //         }
    //     } else {
    //         nfc3_count_redundant = 0;
    //     }
    // }
    // // nfc reader 4
    // reader_result = nfc4_read();
    // reader_no = 4;
    // nfc_reader_idx = reader_no - 1;
    // supposed_instance_idx = reader_correct_idx[nfc_reader_idx];
    // if( supposed_instance_idx != -1) { // supposed to have nfc/server being detected
    //     if (reader_result == "NO_NFC") { // !nfc.tagPresent(...)
    //         if (nfc4_count_missing >= ALERT_THRESHOLD) {
    //             Serial.println("Reader 4: no nfc sticker");
    //             alert_missing_instance_array.push_back(supposed_instance_idx);
    //         } else {
    //             Serial.println("Reader 4: detecting...");
    //             nfc4_count_missing++;
    //         }
    //     } else {
    //         Serial.println("Reader 4: sticker exists");
    //         nfc4_count_missing = 0;
    //         String instance_id = rack_json["data"][supposed_instance_idx]["id"];
    //         if (reader_result.indexOf(instance_id) == -1) { // instance/server id does not exist(match) in nfc sticker
    //             if (nfc4_count_incorrect >= ALERT_THRESHOLD) {
    //                 Serial.println("Reader 4: incorrect sticker contetnt (server id)");
    //                 nfc4_write(instance_id);
    //                 alert_incorrect_instance_array.push_back(supposed_instance_idx);
    //                 // postServerAssetTag(instance_id, nfc1_getuid());
    //                 Serial.println("Post nfc4");
    //             } else {
    //                 nfc4_count_incorrect++;
    //             }
    //         } else {
    //             Serial.println("Reader 4: sticker correct");
    //             nfc4_count_incorrect = 0;
    //         }
    //     }
    // } else { // not supposed to have nfc/server being detected
    //     if (reader_result != "NO_NFC") {
    //         if (nfc4_count_redundant >= ALERT_THRESHOLD) {
    //             Serial.println("Reader 4: redundant sticker");
    //             alert_redundant_nfc_array.push_back(nfc_reader_idx);
    //         } else {
    //             nfc4_count_redundant++;
    //         }
    //     } else {
    //         nfc4_count_redundant = 0;
    //     }
    // }
    // // nfc reader 5
    // reader_result = nfc5_read();
    // reader_no = 5;
    // nfc_reader_idx = reader_no - 1;
    // supposed_instance_idx = reader_correct_idx[nfc_reader_idx];
    // if( supposed_instance_idx != -1) { // supposed to have nfc/server being detected
    //     if (reader_result == "NO_NFC") { // !nfc.tagPresent(...)
    //         if (nfc5_count_missing >= ALERT_THRESHOLD) {
    //             Serial.println("Reader 5: no nfc sticker");
    //             alert_missing_instance_array.push_back(supposed_instance_idx);
    //         } else {
    //             Serial.println("Reader 5: detecting...");
    //             nfc5_count_missing++;
    //         }
    //     } else {
    //         Serial.println("Reader 5: sticker exists");
    //         nfc5_count_missing = 0;
    //         String instance_id = rack_json["data"][supposed_instance_idx]["id"];
    //         if (reader_result.indexOf(instance_id) == -1) { // instance/server id does not exist(match) in nfc sticker
    //             if (nfc5_count_incorrect >= ALERT_THRESHOLD) {
    //                 Serial.println("Reader 5: incorrect sticker contetnt (server id)");
    //                 nfc5_write(instance_id);
    //                 alert_incorrect_instance_array.push_back(supposed_instance_idx);
    //                 // postServerAssetTag(instance_id, nfc1_getuid());
    //                 Serial.println("Post nfc5");
    //             } else {
    //                 nfc5_count_incorrect++;
    //             }
    //         } else {
    //             Serial.println("Reader 5: sticker correct");
    //             nfc5_count_incorrect = 0;
    //         }
    //     }
    // } else { // not supposed to have nfc/server being detected
    //     if (reader_result != "NO_NFC") {
    //         if (nfc5_count_redundant >= ALERT_THRESHOLD) {
    //             Serial.println("Reader 5: redundant sticker");
    //             alert_redundant_nfc_array.push_back(nfc_reader_idx);
    //         } else {
    //             nfc5_count_redundant++;
    //         }
    //     } else {
    //         nfc5_count_redundant = 0;
    //     }
    // }
    
    
    // alerts mqtt publish
    if (alert_missing_instance_array.size() > 0) {
        int arr[alert_missing_instance_array.size()];
        std::copy(alert_missing_instance_array.begin(), alert_missing_instance_array.end(), arr);
        DynamicJsonDocument alert_json(1024);
        alert_json["id"] = 3;
        for (int i = 0; i < alert_missing_instance_array.size(); i++) {
            alert_json["content"][i] = rack_json["data"][arr[i]]["id"];
        }
        String alert_string = "";
        serializeJson(alert_json, alert_string);
        Serial.println("Missing: " + alert_string);
        mqtt_pub(alert_string);
    }
    if (alert_incorrect_instance_array.size() > 0) {
        int arr[alert_incorrect_instance_array.size()];
        std::copy(alert_incorrect_instance_array.begin(), alert_incorrect_instance_array.end(), arr);
        DynamicJsonDocument alert_json(1024);
        alert_json["id"] = 3;
        for (int i = 0; i < alert_incorrect_instance_array.size(); i++) {
            alert_json["content"][i] = rack_json["data"][arr[i]]["id"];
        }
        String alert_string = "";
        serializeJson(alert_json, alert_string);
        Serial.println("Incorrect: " + alert_string);
        mqtt_pub(alert_string);
    }
    if (alert_redundant_nfc_array.size() > 0) {
        int arr[alert_redundant_nfc_array.size()];
        std::copy(alert_redundant_nfc_array.begin(), alert_redundant_nfc_array.end(), arr);
        DynamicJsonDocument alert_json(1024);
        alert_json["id"] = 3;
        for (int i = 0; i < alert_redundant_nfc_array.size(); i++) {
            alert_json["content"][i] = arr[i]+1;
        }
        String alert_string = "";
        serializeJson(alert_json, alert_string);
        Serial.println("Redundant: " + alert_string);
        mqtt_pub(alert_string);
    }
}

void postServerAssetTag(String id, String nfc_uid) {
    DynamicJsonDocument attribute_json(1024);
    attribute_json["id"] = id;
    attribute_json["name"] = "asset_tag";
    attribute_json["value"] = nfc_uid;
    DynamicJsonDocument post_json(1024);
    post_json["id"] = "cd4df3b8-2b9d-4aae-b047-85181022cefa";
    post_json["model_id"] = "5f50592c-44e2-11de-9ac7-000d566af2f2";
    post_json["name"] = "NFC server demo";
    post_json["description"] = "test";
    post_json["attributes"] = attribute_json;
    String post_serial = "";
    serializeJson(post_json, post_serial);
    Serial.println(post_serial);
    // http_post("/devices", post_serial, true, http_username, http_password);
}