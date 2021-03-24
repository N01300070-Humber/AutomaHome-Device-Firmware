/*
 * Author: Jay Patel & Rohan Mir
 * 
 * Target Device: Thermostat (HVAC Interface)
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
const String PATH_TIMESTAMP = "/timestamp";
const String PATH_TEMPERATURE = "/temperature";
const String PATH_HUMIDITY = "/humidity";
const String PATH_COMPRESSOR = "/compressor";
const String PATH_FAN = "/fan";
const String PATH_REVERSE = "/reverse";

// Timing Variables
unsigned long fbNextLoop = 0;
#define FIREBASE_LOOP_INTERVAL 5000
#define READ_TIMEOUT 2500

int messageResult = 0;
String message = "";

// Firebase Realtime Database Data
//String dataTest = "";

float temperature = 0.0;
float humidity = 0.0;


//GPIO0 Pin is to be unplugged after uploading
void setup() 
{
   // Turn on Serial so we can verify expected colors via Serial Monitor
    Serial.begin(115200);
    delay(1000);
    
    // Connect to Wi-Fi
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    
    // Display connection status
    Serial.print("\nConnecting to Wi-Fi");
    
    while (WiFi.status() != WL_CONNECTED) 
    {
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

    fbAuth.token.uid = "Thermostat01"; //Manually assigned for now

    fbAuth.token.claims.add("device", true);

    // Setup Firebase connection
    Firebase.reconnectWiFi(true);
    fbData.setResponseSize(4096);
    Firebase.begin(&fbConfig, &fbAuth);

    fbPath = PATH_START + fbAuth.token.uid.c_str() + PATH_END;
}




void loop() 
{
  if (Serial.available()) 
  {
    messageResult = readIncomingMessage();

    if (messageResult == -1) {
      Serial.println("Error reading message: timed out waiting for new character");
    } else if (messageResult < -1) {
      Serial.print("Error reading message: an unknown error occured with code ");
      Serial.println(messageResult);
    }

    Serial.println("Error reading message: Message is not data");
    Serial.println(message);

    if (messageResult == 1) {
      Serial.println("Setting Sensor Data");
      updateTempHumid();
    }
  }
   
}



/*String getValue(String data, char separator, int index)
{
    int found = 0;
    int strIndex[] = { 0, -1 };
    int maxIndex = data.length() - 1;

    for (int i = 0; i <= maxIndex && found <= index; i++) {
        if (data.charAt(i) == separator || i == maxIndex) {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i+1 : i;
        }
    }
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}*/




int readIncomingMessage(void) 
{
  unsigned long millisReadTimeout = millis() + READ_TIMEOUT;
  char currChar = 0;
  String intString = "";
  int numCommas = 0;
  boolean notData = false;

  message = "";

  while (millis() < millisReadTimeout) 
  {
    if (Serial.available() > 0) 
    {
      currChar = Serial.read();

      if (currChar == '\n' || currChar == '\r') 
      {
        if (notData) 
        {
          return 0; // code 0 = finished reading log message
        }
        
        humidity = intString.toFloat();
        //Serial.print("Set blue brightness variable to ");
        //Serial.println(blueBright);
        return 1; // code 1 = finished reading and interpreting data message
      }
      
      else if (!notData) 
      {
        if (isDigit(currChar)) 
        {
          intString += currChar;
        }
        else if (currChar == ',') 
        {
          numCommas++;
          
          if(numCommas == 1) 
          {
            temperature = intString.toFloat();
            //Serial.print("Set red brightness variable to ");
            //Serial.println(redBright);      
          } 
          
          else 
          {
            Serial.println("message is not data: too many commas");
            notData = true;
          }
          
          intString = "";
        }
        
        else 
        {
          //Serial.print("message is not data: char '");
          //Serial.print(currChar);
          //Serial.print("' is not used in data\n");
          notData = true;
        }
      }

      if (currChar != '\r')
      {
      message += currChar;
      millisReadTimeout = millis() + READ_TIMEOUT;
      }
    }
    
    else 
    {
      yield();
    }
  }

  return -1; // code -1 = timed out waiting for new character
}






void updateTempHumid() 
{
    String path;
    String message;

  if(Serial.available()) 
  {
    message = readIncomingMessage();
    
    
    path = fbPath + PATH_TEMPERATURE;
    if (Firebase.RTDB.set(&fbData, path.c_str(), temperature)) 
    {
        Serial.printf("\nSuccessfully updated %s to %.1f\n", path.c_str(), temperature);
    } 
    else 
    {
        Serial.printf("Failed to update %s: %s\n", path.c_str(), fbData.errorReason().c_str());
    }

   path = fbPath + PATH_HUMIDITY;
    if (Firebase.RTDB.set(&fbData, path.c_str(), humidity))
    {
        Serial.printf("\nSuccessfully updated %s to %.1f\n", path.c_str(), humidity);
    } 
    else 
    {
        Serial.printf("Failed to update %s: %s\n", path.c_str(), fbData.errorReason().c_str());
    }
  }
    updateTimeStamp();
}





void updateTimeStamp(void) 
{
    String path;
    path = fbPath + PATH_TIMESTAMP;
    if (Firebase.RTDB.setTimestamp(&fbData, path.c_str())) 
    {
        Serial.printf("Successfully updated %s to current time\n", path.c_str());
    }
    else 
    {
        Serial.printf("Failed to update %s: %s\n", path.c_str(), fbData.errorReason().c_str());
    }

    Serial.print("\n");
}






void getHVAC() 
{
    String path;
    bool compressor;
    bool reverse;
    bool fan;
    
    path = fbPath + PATH_COMPRESSOR;
    if (Firebase.RTDB.getBool(&fbData, path.c_str())) 
    {
      Serial.printf("Successfully read %s\n", path.c_str());
      compressor = fbData.boolData();
    } 
    else 
    {
      Serial.printf("Failed to update %s: %s\n", path.c_str(), fbData.errorReason().c_str());
    }
   
  
    path = fbPath + PATH_REVERSE;
    if (Firebase.RTDB.getBool(&fbData, path.c_str())) 
    {
      Serial.printf("Successfully read %s\n", path.c_str());
      reverse = fbData.boolData();
    } 
    else 
    {
      Serial.printf("Failed to update %s: %s\n", path.c_str(), fbData.errorReason().c_str());
    }
  
  
    path = fbPath + PATH_FAN;
    if (Firebase.RTDB.getBool(&fbData, path.c_str())) 
    {
      Serial.printf("Successfully read %s\n", path.c_str());
      fan = fbData.boolData();
    } 
    else 
    {
      Serial.printf("Failed to update %s: %s\n", path.c_str(), fbData.errorReason().c_str());
    }

    Serial.printf("%d, %d, %d\n",compressor, reverse, fan);
    delay(5000);  
}
