const boolean IS_COMMON_ANODE = false; 

#define RGB_RED_PIN = PA10;
#define RGB_GREEN_PIN = PA9;
#define RGB_BLUE_PIN = PA8;
const int DELAY_MS = 1000; // delay in ms between changing colors

void setup() {
  // Set the RGB pins to output
  pinMode(RGB_RED_PIN, OUTPUT);
  pinMode(RGB_GREEN_PIN, OUTPUT);
  pinMode(RGB_BLUE_PIN, OUTPUT);

  // Turn on Serial so we can verify expected colors via Serial Monitor
  Serial.begin(9600);
  Serial2.begin(115200);
}

void loop() {

   if (Serial.available() > 0) 
  {
      char state = Serial.read();
      
      if (state == 'H' || state == 'h')
      {
         Serial.print("LED IS ON ");
         setRgbLedColor(HIGH, HIGH, HIGH);
         Serial2.print ("RGB ON"); //send result to ESP8266
         delay(1000);
      }

    // red
      if (state == 'R' || state == 'r')
      {
      Serial.print("Red: ");
      setRgbLedColor(HIGH, LOW, LOW);
      Serial2.print ("Red ON"); //send result to ESP8266
      delay(DELAY_MS);  
      }
  
      // green
      if (state == 'G' || state == 'g')
      {
      Serial.print("Green: ");
      setRgbLedColor(LOW, HIGH, LOW);
      Serial2.print ("Green ON"); //send result to ESP8266
      delay(DELAY_MS);  
      }
  
      // blue
      if (state == 'B' || state == 'b')
      {
      Serial.print("Blue: ");
      setRgbLedColor(LOW, LOW, HIGH);
      Serial2.print ("Blue ON"); //send result to ESP8266
      delay(DELAY_MS); 
      }     
              
      if (state == 'L' || state == 'l')
      {
         Serial.print("LED IS OFF ");
         setRgbLedColor(LOW, LOW, LOW);
         Serial2.print ("RGB OFF"); //send result to ESP8266
      }
     }
    if (Serial2.available()>0){
    Serial.print((char) Serial2.read()); //Forward all received messages from ESP8266 to main serial
      }  
}

/**
 * setRgbLedColor takes in either HIGH or LOW for red, green, and blue.

 */
void setRgbLedColor(int red, int green, int blue)
{
  // If a common anode LED, invert values
  if(IS_COMMON_ANODE == true){
    red = !red;
    green = !green;
    blue = !blue;
  }

  Serial.print(red);
  Serial.print(", ");
  Serial.print(green);
  Serial.print(", ");
  Serial.println(blue);
  
  digitalWrite(RGB_RED_PIN, red);
  digitalWrite(RGB_GREEN_PIN, green);
  digitalWrite(RGB_BLUE_PIN, blue);  
}
