const boolean IS_COMMON_ANODE = false; 

const int RGB_RED_PIN = 5;
const int RGB_GREEN_PIN = 4;
const int RGB_BLUE_PIN = 6;
const int DELAY_MS = 1000; // delay in ms between changing colors

void setup() {
  // Set the RGB pins to output
  pinMode(RGB_RED_PIN, OUTPUT);
  pinMode(RGB_GREEN_PIN, OUTPUT);
  pinMode(RGB_BLUE_PIN, OUTPUT);

  // Turn on Serial so we can verify expected colors via Serial Monitor
  Serial.begin(9600); 
}

void loop() {

   if (Serial.available() > 0) 
  {
      char state = Serial.read();
      
      if (state == 'H' || state == 'h')
      {
         Serial.print("LED IS ON ");
         setRgbLedColor(HIGH, HIGH, HIGH);
         delay(1000);
      }

    // red
      if (state == 'R' || state == 'r')
      {
      Serial.print("Red: ");
      setRgbLedColor(HIGH, LOW, LOW);
      delay(DELAY_MS);  
      }
  
      // green
      if (state == 'G' || state == 'g')
      {
      Serial.print("Green: ");
      setRgbLedColor(LOW, HIGH, LOW);
      delay(DELAY_MS);  
      }
  
      // blue
      if (state == 'B' || state == 'b')
      {
      Serial.print("Blue: ");
      setRgbLedColor(LOW, LOW, HIGH);
      delay(DELAY_MS); 
      }     
      
      // purple
      if (state == 'P' || state == 'p')
      {
      Serial.print("Purple: ");
      setRgbLedColor(HIGH, LOW, HIGH);
      delay(DELAY_MS);
      }
      
      // turqoise
      if (state == 'T' || state == 't')
      {
      Serial.print("Turqoise: ");
      setRgbLedColor(LOW, HIGH, HIGH);
      delay(DELAY_MS);
      }
      
      // yellow
      if (state == 'Y' || state == 'y')
      {
      Serial.print("Yellow: ");
      setRgbLedColor(HIGH, HIGH, LOW);
      delay(DELAY_MS); 
      }   
      
      if (state == 'L' || state == 'l')
      {
         Serial.print("LED IS OFF ");
         setRgbLedColor(LOW, LOW, LOW);
      }
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
