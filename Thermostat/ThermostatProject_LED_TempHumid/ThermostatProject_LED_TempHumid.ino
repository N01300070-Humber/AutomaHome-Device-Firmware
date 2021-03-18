
/*
 * Author: Jay Patel, Rohan Mir
 * 
 * Target Device: Thermostat (HVAC Interface)
 * Target Board: STM32
 */

 #include <DHT.h> // For use with the DHT sensor

//PA1 = DHT11 Sensor
//PB13 = Red LED (Compressor)
//PB14 = Yellow LED (Reverse Value)
//PB15 = Green LED (Cooling Fan)


const boolean IS_COMMON_ANODE = false; 
DHT dht(PA1, DHT11);

void setup() 
{
   // Set the RGB pins to output
  pinMode(PB13, OUTPUT);
  pinMode(PB14, OUTPUT);
  pinMode(PB15, OUTPUT);
  dht.begin();
}



void loop() 
{

  if (Serial.available() > 0) 
  {
      char state = Serial.read();

      if (state == 'T' || state == 't')
      {
        displayTemp();
        delay(1000);  
      }
      if (state == 'H' || state == 'h')
      {
         Serial.print("LED IS ON ");
         HVAC_State(HIGH, HIGH, HIGH);
         delay(1000);
      }
      
      if (state == 'R' || state == 'r')
      {
      // red
        Serial.print("Red: ");
        HVAC_State(HIGH, LOW, LOW);
        delay(1000);
      }

      if (state == 'Y' || state == 'y')
      {
       // yellow
        Serial.print("Yellow: ");
        HVAC_State(LOW, HIGH, LOW);
        delay(1000);
      }

      
      if (state == 'G' || state == 'g')
      {
        // green
        Serial.print("Green: ");
         HVAC_State(LOW, LOW, HIGH);
        delay(1000);
      }
    
      if (state == 'L' || state == 'l')
      {
         Serial.print("LED IS OFF ");
         HVAC_State(LOW, LOW, LOW);
      }
    }
}


/**
 * The serial reads Red('R', 'r'), Yellow('Y', 'y'), Green('G', 'g') and on/off switches which are High('H' 'h') and Low('L', 'l').
 * This function is based on https://gist.github.com/jamesotron/766994
 */
void  HVAC_State(int red, int green, int yellow)
{
  // If a common anode LED, invert values
  if(IS_COMMON_ANODE == true)
  {
    red = !red;
    green = !green;
    yellow = !yellow;
  }

  Serial.print(red);
  Serial.print(", ");
  Serial.print(green);
  Serial.print(", ");
  Serial.println(yellow);
  
  digitalWrite(PB13, red);
  digitalWrite(PB14, yellow);
  digitalWrite(PB15, green);  
}

void displayTemp()
{
  float h = dht.readHumidity();
  float tc = dht.readTemperature();
  float tf = dht.convertCtoF(tc);
  
  Serial.print("\nTemperature (Celsius): ");
  Serial.print(tc);
  Serial.print("\nTemperature (Fahrenheit): ");
  Serial.print(tf);
  Serial.print("\nHumidity: ");
  Serial.print(h);
  Serial.print("\n");
}
