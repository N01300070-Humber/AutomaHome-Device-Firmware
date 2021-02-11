#define LED1 PC13
#define LED2 PA4
#define IRE1 PB0 //Timer chanel: T3C3
#define IRE2 PB1 //Timer chanel: T3C4
#define IRR1 PA6
#define IRR2 PA7
#define TX PA2
#define RX PA3

int loopIteration = 0;

void setup() {
  pinMode(LED1, OUTPUT); //The LED on the STM32 board is reversed LOW = ON, HIGH = OFF
  pinMode(LED2, OUTPUT);
  pinMode(IRE1, PWM);
  pinMode(IRE2, PWM);
  pinMode(IRR1, INPUT);
  pinMode(IRR2, INPUT);

  //IR Emitter configuration
  HardwareTimer timerIR(3);
  timerIR.pause();
  timerIR.setOverflow(1895);
  timerIR.refresh();
  timerIR.resume();

  pwmWrite(IRE1, 470);
  pwmWrite(IRE2, 470);
}

void loop() {
  Serial.print("IRR1 = ");
  Serial.print(analogRead(IRR1), DEC);
  Serial.print(",\n");
  Serial.print("IRR2 = ");
  Serial.print(analogRead(IRR2), DEC);
  Serial.print("\n\n");
  
  if (loopIteration % 100 == 0) {
    bool setPin = loopIteration % 20 == 0;
    
    digitalWrite(LED1, setPin);
    digitalWrite(LED2, setPin);
  }
  
  loopIteration += 1;
  delay(10);
}
