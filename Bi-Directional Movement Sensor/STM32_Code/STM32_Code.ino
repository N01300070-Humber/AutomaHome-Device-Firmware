/*
 * Author: Calvin Abel
 * 
 * Target Device: Bi-Directional Movement Sensor
 * Target Baord: STM32
 * 
 * Description:
 * Detects movement and lights up the LED on the side where the movement was detected to be going.
 */

#define LED1 PC13
#define LED2 PA4
#define IRE1 PB0 //Timer chanel: T3C3 (Timer 3, Chanel 3)
#define IRE2 PB1 //Timer chanel: T3C4 (Timer 3, Chanel 4)
#define IRR1 PA6
#define IRR2 PA7
#define TX PA2
#define RX PA3

#define LOWER_THESHHOLD 500
#define HIGHER_THESHHOLD 3500
#define MOVEMENT_DELAY 300

boolean side1 = false;
boolean side2 = false;
boolean movement = false;
unsigned long movementTime = 0;

void setup() {
    pinMode(LED1, OUTPUT); //The LED on the STM32 board is reversed LOW = ON, HIGH = OFF
    pinMode(LED2, OUTPUT);
    pinMode(IRE1, PWM); //IRE1 and IRE2 read at about 4000 when there is either no signal or a constant signal
    pinMode(IRE2, PWM); // The ouput from IRE1 and IRE2 drop only when the constant signal is regained not lost
    pinMode(IRR1, INPUT);
    pinMode(IRR2, INPUT);

    Serial.begin(9600);
    Serial2.begin(115200);
    
    //IR Emitter configuration
    // Set timer 3 on the STM32 to 38000kHz
    HardwareTimer timerIR(3);
    timerIR.pause();
    timerIR.setOverflow(1895);
    timerIR.refresh();
    timerIR.resume();
    
    // Send a constant PWM signal at 25% work
    pwmWrite(IRE1, 470);
    pwmWrite(IRE2, 470);
}

void loop() {
    if (movement) {
        if (movementTime < millis()) {
            movement = false;
            digitalWrite(LED1, !LOW);
            digitalWrite(LED2, LOW);
        }
    } else {
        if (analogRead(IRR1) < LOWER_THESHHOLD && !side1) {
            side1 = true;
//          Serial.println("Change detected on side1");
            if (side2) {
                Serial.println("Detected movemoent going to side1");
                digitalWrite(LED1, !HIGH);
                Serial2.print("left\r\n"); //Send result to ESP8266
            }
        }
        
        if (analogRead(IRR2) < LOWER_THESHHOLD && !side2) {
            side2 = true;
//          Serial.println("Change detected on side2");
            if (side1) {
                Serial.println("Detected movemoent going to side2");
                digitalWrite(LED2, HIGH);
                Serial2.print("right\r\n"); //Send result to ESP8266
            }
        }
        
        if (side1 && side2) {
            movement = true;
            movementTime = millis() + MOVEMENT_DELAY;
            side1 = false;
            side2 = false;
        }
    }
    if (Serial2.available() > 0) {
        Serial.print((char) Serial2.read()); //Forward all received messages from ESP8266 to main serial
    }
}
