int base = 10;
int MAX_DURATION = 5000;

void setup(){
  Serial.begin(9600);
  pinMode(base, OUTPUT);    
}

void loop(){
  if (Serial.available()) {
//    read the most recent byte (which will be from 0 to 255):
    int duration = Serial.parseInt();
//    Serial.println(duration);
    if (duration > MAX_DURATION) {
      duration = MAX_DURATION;
    }
    digitalWrite(base, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(duration);           // wait for 5 seconds
    digitalWrite(base, LOW);    // turn the LED off by making the voltage LOW
  }
}

