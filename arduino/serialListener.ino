int RELAY_ON = LOW;
int RELAY_OFF = HIGH;
int DURATION = 500;
int base = 10;
int state = RELAY_OFF;
unsigned long previousMillis = 0;

void setup(){
  Serial.begin(9600);
  digitalWrite(base, RELAY_OFF);
  pinMode(base, OUTPUT);
}

void loop(){
  if (state == RELAY_ON) {
    unsigned long currentMillis = millis();
    if(currentMillis - previousMillis > DURATION) {
      Serial.println("RELAY_OFF");
      state = RELAY_OFF;
      digitalWrite(base, RELAY_OFF);
    }
  }
}

void serialEvent() {
  boolean found = Serial.find("OPEN");
  if (found && state == RELAY_OFF) {
    Serial.println("RELAY_ON");
    state = RELAY_ON;
    digitalWrite(base, RELAY_ON);
    previousMillis = millis();
  }
}
