#define LED1 PC13
#define LED2 PA4
#define IRE1 PB0
#define IRE2 PB1
#define IRR1 PA6
#define IRR2 PA7
#define TX PA2
#define RX PA3

int loopIteration = 0;

void setup() {
  pinMode(LED1, OUTPUT); //LED on board is revesed LOW = ON, HIGH = OFF
  pinMode(LED2, OUTPUT);
  pinMode(IRE1, OUTPUT);
  pinMode(IRE2, OUTPUT);
  pinMode(IRR1, INPUT);
  pinMode(IRR2, INPUT);
}

void loop() {
  Serial.print("IRR1 = ");
  Serial.print(analogRead(IRR1), DEC);
  Serial.print(",\n");
  Serial.print("IRR2 = ");
  Serial.print(analogRead(IRR2), DEC);
  Serial.print("\n\n");
  
  if (loopIteration % 10 == 0) {
    bool setPin = loopIteration % 20 == 0;

    digitalWrite(LED1, !setPin);
    digitalWrite(IRE1, setPin);
    
    digitalWrite(LED2, !setPin);
    digitalWrite(IRE2, !setPin);
  }
  
  loopIteration += 1;
  delay(100);
}
