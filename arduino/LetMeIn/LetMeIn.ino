/* 
  LetMeIn door opener. When OPEN command is received on serial port door is opened.
  eZ430-RF2500T wireless board is connected to digital input, when that digital in is high door should be opened.
  
 */
 
const int RELAY_ON = LOW;
const int RELAY_OFF = HIGH;
const int RELAY_SIGNAL_DURATION = 250;
const int relay = 10;
const int signalLed = 13;
const int MSP430 = 2;

int state = RELAY_OFF;
unsigned long relaySignalStartMilis = 0;

void setup() {
  digitalWrite(relay, RELAY_OFF);
  pinMode(relay, OUTPUT);
  pinMode(signalLed, OUTPUT);
  pinMode(MSP430, INPUT);
  
  Serial.begin(9600);
  Serial.println("Program start.");
}

void loop(){
  turnRelayOff();
  checkMSP430();
}

void serialEvent() {
  boolean found = Serial.find("OPEN");
  if (found) {
    Serial.println("Received OPEN command via Serial");
    turnRelayOn();
  }
}

void turnRelayOn() {
  if (state == RELAY_OFF) {
    Serial.println("RELAY_ON");
    state = RELAY_ON;
    digitalWrite(relay, RELAY_ON);
    relaySignalStartMilis = millis();
    digitalWrite(signalLed, HIGH);
  }
  else {
    Serial.println("Relay is already on");
  }
}

void turnRelayOff() {
  if (state == RELAY_ON) {
    unsigned long currentMillis = millis();
    if(currentMillis - relaySignalStartMilis > RELAY_SIGNAL_DURATION) {
      Serial.println("RELAY_OFF");
      state = RELAY_OFF;
      digitalWrite(relay, RELAY_OFF);
      digitalWrite(signalLed, LOW);
      delay(500);
    }
  }
}

void checkMSP430() {
  int inputValue = digitalRead(MSP430);
  if (inputValue == HIGH) {
    Serial.println("Received OPEN command from MSP430 via RF");
    turnRelayOn();
    delay(10);
  }
}
