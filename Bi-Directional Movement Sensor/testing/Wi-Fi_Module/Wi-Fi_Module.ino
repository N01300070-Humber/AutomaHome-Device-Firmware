/*
 * Author: Calvin Abel
 * 
 * Target Device: Bi-Directional Movement Sensor
 * Target Baord: STM32
 * 
 * Description:
 * Simple testing code for communication between the STM32 and ESP8266.
 * Sends what is typed in the Serial Monitor to the ESP8266 and shows the response.
 * Good for testing commands or configuring the ESP8266 manualy.
 */

#define LED1 PC13
#define LED2 PA4
#define IRE1 PB0 //Timer chanel: T3C3 (Timer 3, Chanel 3)
#define IRE2 PB1 //Timer chanel: T3C4 (Timer 3, Chanel 4)
#define IRR1 PA6
#define IRR2 PA7
//#define TX PA2
//#define RX PA3

void setup() {
  pinMode(LED1, OUTPUT); //The LED on the STM32 board is reversed LOW = ON, HIGH = OFF
  pinMode(LED2, OUTPUT);
  pinMode(IRE1, PWM); //IRE1 and IRE2 read at about 4000 when there is either no signal or a constant signal
  pinMode(IRE2, PWM); // The ouput from IRE1 and IRE2 drop only when the constant signal is regained not lost
  pinMode(IRR1, INPUT);
  pinMode(IRR2, INPUT);

  Serial.begin(9600);
  Serial2.begin(115200);

  while(!Serial2 || !Serial) { // Wait for Serial connections to initialize
    delay(1);
  }

//  Serial.println(sendCommand("AT+RST"));
//  delay(2000);
//  sendCommand("AT+CWMODE=1");
//  sendCommand("AT+CWJAP=\"LinkedinPark\",\"mct052029\"");
//  sendCommand("AT+CIFSR");
}

void loop() {
  if (Serial.available() > 0) {
    Serial2.print((char) Serial.read());
  }
  
  if (Serial2.available() > 0) {
    Serial.print((char) Serial2.read());
  }
}

String sendCommand(String command) {
  String message = "";
  String fullReply = "";
  
  Serial2.print(command);
  Serial2.print("\r\n");

  while(true) {
    while(Serial2.available() == 0) { // Wait for reply
      delay(1);
    }
    while(Serial2.available() > 0) { // Read reply
      message += (char) Serial2.read();
    }
    fullReply += message;
    Serial.print(message);
    if (message == "OK" || message == "ERROR") {
      break;
    }
  }
  
  Serial.print("\n\n");
  return fullReply;
}
