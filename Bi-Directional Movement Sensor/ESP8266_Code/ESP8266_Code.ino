/*
 * Author: Calvin Abel
 * 
 * Target Device: Bi-Directional Movement Sensor
 * Target Board: ESP8266
 */
 
#include <ESP8266WiFi.h>
#include <Firebase_ESP_Client.h>
#include "firebaseCredentials.h" //Contains FIREBASE_HOST, FIREBASE_API_KEY, FIREBASE_PROJECT_ID, FIREBASE_CLIENT_EMAIL, FIREBASE_PRIVATE_KEY
// firebaseCredentials.h temporarily contains WIFI_SSID and WIFI_PASSWORD


void updateRTDB(void);
String readInput(void);


// Firebase Objects
FirebaseData fbData;
FirebaseAuth fbAuth;
FirebaseConfig fbConfig;
struct token_info_t tokenInfo;

// Firebase Path
String fbPath;
const String PATH_START = "/devices/";
const String PATH_END = "/data/log";
const String KEY_DIRECTION = "direction";
const String PATH_TIMESTAMP = "/timestamp";

// Timing Variables
unsigned long fbNextLoop = 0;
#define FIREBASE_LOOP_INTERVAL 5000
#define READ_TIMEOUT 250

// Firebase Realtime Database Data
String dataDirection = "";

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
    if (Serial.available() > 0) {
        // Check staus of token used for connection
        tokenInfo = Firebase.authTokenInfo();
        if (tokenInfo.status == token_status_error) {
            Serial.printf("Token Error: %s (%d)\n\n", tokenInfo.error.message.c_str(), tokenInfo.error.code);
        } else {
            dataDirection = readInput();
            updateRTDB();
        }
    }
    
    if (millis() >= fbNextLoop) {
        fbNextLoop = millis() + FIREBASE_LOOP_INTERVAL;

        // Check staus of token used for connection
        tokenInfo = Firebase.authTokenInfo();
        if (tokenInfo.status == token_status_error) {
            Serial.printf("Token Error: %s (%d)\n\n", tokenInfo.error.message.c_str(), tokenInfo.error.code);
        }
    }
}

void updateRTDB(void) {
    String path;
    FirebaseJson json;

    // Fill out JSON object
    json.set(KEY_DIRECTION, dataDirection);

    // Add JSON object to database as new entry
    if (Firebase.RTDB.pushJSON(&fbData, fbPath.c_str(), &json)) {
        Serial.printf("Successfully created new entry\n");

        // Add current server timestamp to new entry
        path = fbData.dataPath() + "/" + fbData.pushName() + PATH_TIMESTAMP;
        if (Firebase.RTDB.setTimestamp(&fbData, path.c_str())) {
            Serial.printf("Successfully set timestamp\n");
        } else {
            Serial.printf("Failed to set timestamp: %s\n", fbData.errorReason().c_str());
        }
    } else {
        Serial.printf("Failed to create new entry: %s\n", fbData.errorReason().c_str());
    }
    
    Serial.print("\n");
}

String readInput(void) {
    unsigned long millisReadTimeout = millis() + READ_TIMEOUT;
    char prevChar = 0;
    char currChar = 0;
    String message = "";

    Serial.print("\nRecieved Message:\n");
    while (millis() < millisReadTimeout) {
        if (Serial.available() > 0) {
            currChar = Serial.read();
            
            if (!(currChar == '\n' || currChar == '\r')) {
                Serial.print(currChar);
                message += currChar;
                millisReadTimeout = millis() + READ_TIMEOUT;
            }
            
            if (currChar == '\n' && prevChar == '\r' || currChar == '\r' && prevChar == '\n') {
                Serial.print("\nEnd of message: newline and return character terminated\n");
                return message;
            }
            
            prevChar = currChar;
        }
        else {
            yield();
        }
    }

    Serial.print("\nEnd of message: timed out waiting for new character\n");
    return message;
}
