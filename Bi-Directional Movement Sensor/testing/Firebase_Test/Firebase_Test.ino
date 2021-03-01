/*
 * Author: Calvin Abel
 * 
 * Target Device: Any
 * Target Baord: ESP8266
 */
 
#include <ESP8266WiFi.h>
#include <Firebase_ESP_Client.h>
#include "firebaseCredentials.h" //Contains FIREBASE_HOST, FIREBASE_API_KEY, FIREBASE_PROJECT_ID, FIREBASE_CLIENT_EMAIL, FIREBASE_PRIVATE_KEY
// firebaseCredentials.h temporarily contains WIFI_SSID and WIFI_PASSWORD


void updateRTDB(void);


// Firebase Objects
FirebaseData fbData;
FirebaseAuth fbAuth;
FirebaseConfig fbConfig;

// Firebase Path
String fbPath;
const String PATH_START = "/devices/";
const String PATH_END = "/data";
const String PATH_TEST = "/test";
const String PATH_TIMESTAMP = "/timestamp";

// Loop Variables
unsigned long fbNextLoop = 0;
#define FIREBASE_LOOP_INTERVAL 5000

// Firebase Realtime Database Data
int dataTest = 0;

void setup() {
    Serial.begin(115200);
    delay(1000);
    
    // Connect to Wi-Fi
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    // Display connection status
    Serial.print("\nConnecting to Wi-Fi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(250); //Wait until connection successful
        Serial.print(".");
    }
    Serial.print("\nSuccessfully connected to Wi-FI\n\n");
    
    // Set Firebase credentials
    fbConfig.host = FIREBASE_HOST;
    fbConfig.api_key = FIREBASE_API_KEY;

    fbConfig.service_account.data.client_email = FIREBASE_CLIENT_EMAIL;
    fbConfig.service_account.data.project_id = FIREBASE_PROJECT_ID;
    fbConfig.service_account.data.private_key = FIREBASE_PRIVATE_KEY;

    fbAuth.token.uid = "Movement01"; //Manually assigned for now

    fbAuth.token.claims.add("device", true);

    // Setup Firebase connection
    Firebase.reconnectWiFi(true);
    fbData.setResponseSize(4096);
    Firebase.begin(&fbConfig, &fbAuth);

    fbPath = PATH_START + fbAuth.token.uid.c_str() + PATH_END;
}

void loop() {
    if (millis() >= fbNextLoop) {
        fbNextLoop = millis() + FIREBASE_LOOP_INTERVAL;

        // Check staus of token used for connection
        struct token_info_t tokenInfo = Firebase.authTokenInfo();
        if (tokenInfo.status == token_status_error) {
            Serial.printf("Token Error: %s\n\n", tokenInfo.error.message.c_str());
        }
        else {
            updateRTDB();
        }
    }
}

void updateRTDB(void) {
    String path;
    
    dataTest++;
    path = fbPath + PATH_TEST;
    if (Firebase.RTDB.set(&fbData, path.c_str(), dataTest)) {
        Serial.printf("Successfully updated %s to %d\n", path.c_str(), dataTest);
    } else {
        Serial.printf("Failed to update %s: %s\n", path.c_str(), fbData.errorReason().c_str());
    }

    path = fbPath + PATH_TIMESTAMP;
    if (Firebase.RTDB.setTimestamp(&fbData, path.c_str())) {
        Serial.printf("Successfully updated %s to current time\n", path.c_str());
    } else {
        Serial.printf("Failed to update %s: %s\n", path.c_str(), fbData.errorReason().c_str());
    }

    Serial.print("\n");
}
