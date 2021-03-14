/*
 * Author: Y Thien Nguyen
 * 
 * Target Device: RGB LED
 * Target Board: ESP8266
 */

#include <ESP8266WiFi.h>
#include <Firebase_ESP_Client.h>
#include "firebaseCredentials.h" //Contains FIREBASE_HOST, FIREBASE_API_KEY, FIREBASE_PROJECT_ID, FIREBASE_CLIENT_EMAIL, FIREBASE_PRIVATE_KEY
// firebaseCredentials.h temporarily contains WIFI_SSID and WIFI_PASSWORD

// Firebase Objects
FirebaseData fbData;
FirebaseAuth fbAuth;
FirebaseConfig fbConfig;
struct token_info_t tokenInfo;

// Firebase Path
String fbPath;
const String PATH_START = "/devices/";
const String PATH_END = "/data";




void setup() 
{
   // Turn on Serial so we can verify expected colors via Serial Monitor
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

    fbAuth.token.uid = "RGBStrip01"; //Manually assigned for now

    fbAuth.token.claims.add("device", true);

    // Setup Firebase connection
    Firebase.reconnectWiFi(true);
    fbData.setResponseSize(4096);
    Firebase.begin(&fbConfig, &fbAuth);

    fbPath = PATH_START + fbAuth.token.uid.c_str() + PATH_END;
}


void loop() 
{
  //Verify that you are connected to the Firebase
  tokenInfo = Firebase.authTokenInfo();
  if (tokenInfo.status == token_status_error) 
  {
     Serial.printf("Token Error: %s (%d)\n\n", tokenInfo.error.message.c_str(), tokenInfo.error.code);
  }
  else
  {
    String path;
    bool red;
    bool green;
    bool blue;
    
    path = fbPath + "/red";
    if (Firebase.RTDB.getBool(&fbData, path.c_str())) 
    {
      Serial.printf("Successfully read %d\n", path.c_str());
      red = fbData.boolData();
    } 
    else 
    {
      Serial.printf("Failed to update %s: %s\n", path.c_str(), fbData.errorReason().c_str());
    }
   
  
    path = fbPath + "/green";
    if (Firebase.RTDB.getBool(&fbData, path.c_str())) 
    {
      Serial.printf("Successfully read %d\n", path.c_str());
      green = fbData.boolData();
    } 
    else 
    {
      Serial.printf("Failed to update %s: %s\n", path.c_str(), fbData.errorReason().c_str());
    }
  
  
    path = fbPath + "/blue";
    if (Firebase.RTDB.getBool(&fbData, path.c_str())) 
    {
      Serial.printf("Successfully read %d\n", path.c_str());
      blue = fbData.boolData();
    } 
    else 
    {
      Serial.printf("Failed to update %s: %s\n", path.c_str(), fbData.errorReason().c_str());
    }

    Serial.printf("%d, %d, %d\n",red, green, blue);
  }
    delay(5000);
}
