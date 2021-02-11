// Serial2 uses pin A2 and A3
//  TX - PA3
//  RX - PA2

void setup() {
  Serial.begin(9600);
  Serial2.begin(115200);

  while(!Serial2 || !Serial) { // Wait for Serial connections to initialize
    delay(1);
  }
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
  String reply = "";
  
  Serial2.print(command);
  Serial2.print("\r\n");

  while(Serial2.available() == 0) { // Wait for reply
    delay(1);
  }
  while(Serial2.available() > 0) { // Read reply
    reply += (char) Serial2.read();
  }

  Serial.print(reply);
  Serial.print("\n\n");
  return reply;
}
