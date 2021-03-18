#define red_led PA8
#define green_led PA10
#define blue_led PA9

#define READ_TIMEOUT 250

int messageResult = 0;
String message = "";


int redBright = 0;
int greenBright = 0;
int blueBright = 0;

void setup()
{
  pinMode(red_led, OUTPUT);
  pinMode(green_led, OUTPUT);
  pinMode(blue_led, OUTPUT);
  
  analogWrite(red_led, 0);
  analogWrite(green_led, 0);
  analogWrite(blue_led, 0);
  Serial.begin(9600);
  Serial2.begin(115200);
}

void loop() {
  if (Serial2.available()) {
    messageResult = readIncomingMessage();

    if (messageResult == -1) {
      Serial.println("Error reading message: timed out waiting for new character");
    } else if (messageResult < -1) {
      Serial.print("Error reading message: an unkown error occured with code ");
      Serial.println(messageResult);
    }

    Serial.print("ESP8266: ");
    Serial.println(message);

    if (messageResult == 1) {
      Serial.println("Setting LED brightnesses:");
      write_leds();
    }

  }
}



void write_leds()
{
  //if (led == 'r'){
  //  Serial.println("Received Red input");
    analogWrite(red_led, redBright);
    Serial.print ("red:");
    Serial.println(redBright);
  //  return;
 // }
 // if (led == 'g'){
  //  Serial.println("Green value");
    analogWrite(green_led, greenBright);
    Serial.print ("green:");
    Serial.println(greenBright);
  //  return;
  //}
 // if (led == 'b'){
   // Serial.println("Blue value");
    analogWrite(blue_led, blueBright);
    Serial.print ("blue:");
    Serial.println(blueBright);
   // return;
  //}
  
  return;
}

int readIncomingMessage(void) {
  unsigned long millisReadTimeout = millis() + READ_TIMEOUT;
  char currChar = 0;
  String intString = "";
  int numCommas = 0;
  boolean notData = false;

  message = "";

  while (millis() < millisReadTimeout) {
    if (Serial2.available() > 0) {
      currChar = Serial2.read();

      if (currChar == '\n') {
        if (notData) {
          return 0; // code 0 = finished reading log message
        }
        blueBright = intString.toInt();
        //Serial.print("Set blue brightness variable to ");
        //Serial.println(blueBright);
        return 1; // code 1 = finished reading and interpreting data message
      }
      else if (!notData) {
        if (isDigit(currChar)) {
          intString += currChar;
        }
        else if (currChar == ',') {
          numCommas++;
          
          if (numCommas == 1) {
            redBright = intString.toInt();
            //Serial.print("Set red brightness variable to ");
            //Serial.println(redBright);
          } else if (numCommas == 2) {
            greenBright = intString.toInt();
            //Serial.print("Set green brightness variable to ");
            //Serial.println(greenBright);
          } else {
            Serial.println("message is not data: too many commas");
            notData = true;
          }
          
          intString = "";
        }
        else {
          //Serial.print("message is not data: char '");
          //Serial.print(currChar);
          //Serial.print("' is not used in data\n");
          notData = true;
        }
      }

      if (currChar != '\r'){
      message += currChar;
      millisReadTimeout = millis() + READ_TIMEOUT;
      }
    }
    else {
      yield();
    }
  }

  return -1; // code -1 = timed out waiting for new character
}
