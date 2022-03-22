#include "trackerlogic.h"


int u_interval = 2;
DynamicJsonDocument rack_json(10240);
int reader_correct_idx [2] = { -1 };

void updateRackInfoJson(){
    String content = http_get("/devices/rdinfo", "501ef728-6e4d-4d8d-b2b4-19ac01fcf96d", true, "admin", "Monit@r#1");
    // String content = "{\"code\": \"1\",\"data\": [{\"description\": \"test\",\"mid\": \"5f50592c-44e2-11de-9ac7-000d566af2f2\",\"upos\": 40.0,\"pid\": \"ca7391ec-d002-11dd-8252-001d091dd9dd\",\"mname\": \"2950III\",\"acl\": 15,\"tid\": \"59e3262e-cd7b-11dd-81bf-001d091dd9dd\",\"sid\": 21115,\"vid\": \"6b16fb0e-cd81-11dd-83bf-001d091dd9dd\",\"is_reserved\": false,\"wo\": false,\"id\": \"cd4df3b8-2b9d-4aae-b047-85181022cefa\",\"sn\": null,\"eups\": null,\"owner\": null,\"side\": \"\",\"enclourse\": null,\"pname\": \"PowerEdge\",\"status_name\": \"Operational\",\"ip\": null,\"tname\": \"Server - Rackmount\",\"psid\": 1670,\"tsid\": 533,\"vsid\": 246,\"shelf\": null,\"msid\": 28436,\"slots\": null,\"vname\": \"Dell\",\"at\": \"at123456\",\"reserved\": false,\"name\": \"NFC server demo\",\"_id\": \"cd4df3b8-2b9d-4aae-b047-85181022cefa\",\"position\": \"40.0 U\",\"_pid_\": null,\"_is_child_\": false,\"status\": 1},{\"description\": null,\"mid\": \"9d2b566a-d52d-11ea-bf0d-b32c7d6d75cf\",\"upos\": 38.0,\"pid\": \"ca7391ec-d002-11dd-8252-001d091dd9dd\",\"mname\": \"R740xd (12D 3.5)\",\"acl\": 15,\"tid\": \"59e3262e-cd7b-11dd-81bf-001d091dd9dd\",\"sid\": 21116,\"vid\": \"6b16fb0e-cd81-11dd-83bf-001d091dd9dd\",\"is_reserved\": false,\"wo\": false,\"id\": \"032d6098-a43e-4d67-8def-2f2f5e823158\",\"sn\": null,\"eups\": null,\"owner\": null,\"side\": \"\",\"enclourse\": null,\"pname\": \"PowerEdge\",\"status_name\": \"Operatjoinional\",\"ip\": null,\"tname\": \"Server - Rackmount\",\"psid\": 1670,\"tsid\": 533,\"vsid\": 246,\"shelf\": null,\"msid\": 54715,\"slots\": null,\"vname\": \"Dell\",\"at\": null,\"reserved\": false,\"name\": \"NFC Server Demo -1\",\"_id\": \"032d6098-a43e-4d67-8def-2f2f5e823158\",\"position\": \"38.0 U\",\"_pid_\": null,\"_is_child_\": false,\"status\": 1}],\"message\": \"Success\"}";
    // String content = "{\"sensor\":\"gps\",\"time\":1351824120,\"data\":[48.756080,2.302038]}";
    deserializeJson(rack_json, content);
}

void updateSupposedReadings(int start_u){
    String temp_string = rack_json["data"];
    int array_length = 0;
    for (int i = 0; i<12; i++){
        String str = rack_json["data"][i];
        if(str!="null"){
            array_length++;
        }else{
            break;
        }
    }
    for(int i = 0; i<array_length; i++) {
        int u_position = rack_json["data"][i]["upos"];
        int reader_num = (u_position - start_u) / u_interval;
        reader_correct_idx [reader_num] = i; // eg.[1, 2, -1] 1&2: index of instance array, -1: supposed no nfc/instance should be detected
    }
}

void rackInfoAlertChecking(){
    int nfc_reader_idx = 0;
    int supposed_instance_idx = 0;
    std::vector<int> alert_missing_instance_array;
    std::vector<int> alert_incorrect_instance_array;
    std::vector<int> alert_redundant_nfc_array;
    String reader_result = "";
    reader_result = nfc1_read();
    nfc_reader_idx = 0;
    supposed_instance_idx = reader_correct_idx[nfc_reader_idx];
    if( supposed_instance_idx != -1) {
        if (reader_result == "nonfc") {
            Serial.println("no nfc1");
            alert_missing_instance_array.push_back(supposed_instance_idx);
        } else {
            String instance_id = rack_json["data"][supposed_instance_idx]["id"];
            if (reader_result.indexOf(instance_id) == -1) {
                nfc1_write(instance_id);
                alert_incorrect_instance_array.push_back(supposed_instance_idx);
            }
        }
    } else {
        if (reader_result != "") {
            alert_redundant_nfc_array.push_back(nfc_reader_idx);
        }
    }
    reader_result = nfc2_read();
    nfc_reader_idx = 1;
    supposed_instance_idx = reader_correct_idx[nfc_reader_idx];
    if( supposed_instance_idx != -1) {
        if (reader_result == "nonfc") {
            Serial.println("no nfc2");
            alert_missing_instance_array.push_back(supposed_instance_idx);
        } else {
            String instance_id = rack_json["data"][supposed_instance_idx]["id"];
            if (reader_result.indexOf(instance_id) == -1) {
                nfc2_write(instance_id);
                alert_incorrect_instance_array.push_back(supposed_instance_idx);
            }
        }
    } else {
        if (reader_result != "") {
            alert_redundant_nfc_array.push_back(nfc_reader_idx);
        }
    }
    
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
        Serial.println("Missing: ");
        Serial.println(alert_string);
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
        Serial.println("Incorrect: ");
        Serial.println(alert_string);
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
        Serial.println("Redundant: ");
        Serial.println(alert_string);
        mqtt_pub(alert_string);
    }
}
