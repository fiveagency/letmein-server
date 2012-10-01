/* 
  LetMeIn door opener. When OPEN command is received on serial port door is opened.
  Detects patterns of knocks and opens the door it if the pattern is correct.
  
  Used part of the code of Secret knock:
  By Steve Hoefer http://grathio.com
  Version 0.1.10.20.10
  Licensed under Creative Commons Attribution-Noncommercial-Share Alike 3.0
  http://creativecommons.org/licenses/by-nc-sa/3.0/us/
  (In short: Do what you want, just be sure to include this line and the four above it, and don't sell it or use it in anything you sell without contacting me.)
   
 */
 
const int RELAY_ON = LOW;
const int RELAY_OFF = HIGH;
const int RELAY_SIGNAL_DURATION = 500;
const int relay = 10;
const int knockSensor = A4;         // Piezo sensor on pin 0.
const int signalLed = 13;

// Tuning constants.  Could be made vars and hoooked to potentiometers for soft configuration, etc.
const int threshold = 3;           // Minimum signal from the piezo to register as a knock
const int rejectValue = 25;        // If an individual knock is off by this percentage of a knock we don't unlock..
const int averageRejectValue = 15; // If the average timing of the knocks is off by this percent we don't unlock.
const int knockFadeTime = 150;     // milliseconds we allow a knock to fade before we listen for another one. (Debounce timer.)

const int maximumKnocks = 10;       // Maximum number of knocks to listen for.
const int knockComplete = 1200;     // Longest time to wait for a knock before we assume that it's finished.

int state = RELAY_OFF;
unsigned long relaySignalStartMilis = 0;

//int secretCode[maximumKnocks] = {50, 100, 100, 0, 0, 0, 0, 0, 0, 0};  // Initial setup: two short, two long
int secretCode[maximumKnocks] = {50, 25, 25, 50, 100, 50, 0, 0, 0, 0};  // Initial setup: "Shave and a Hair Cut, two bits."

int knockReadings[maximumKnocks];   // When someone knocks this array fills with delays between knocks.
int secretKnockCount = 0;

void setup(){
  digitalWrite(relay, RELAY_OFF);
  pinMode(relay, OUTPUT);
  pinMode(signalLed, OUTPUT);
  
  Serial.begin(9600);
  Serial.println("Program start.");
  
  for (int i=0; i<maximumKnocks; i++){
    if (secretCode[i] > 0){  					//todo: precalculate this.
      secretKnockCount++;
    }
  }
}

void loop(){
  turnRelayOff();
  listenForKnock();
}

void serialEvent() {
  boolean found = Serial.find("OPEN");
  if (found) {
    Serial.println("Received OPEN command");
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

void listenForKnock() {
  if (state == RELAY_OFF) {
    // Listen for any knock at all.
    int knockSensorValue = analogRead(knockSensor);
    
    if (knockSensorValue >= threshold){
      listenToSecretKnock();
    }
  }
}

// Records the timing of knocks.
void listenToSecretKnock(){
  Serial.println("knock starting");   

  int i = 0;
  // First lets reset the listening array.
  for (i=0; i<maximumKnocks; i++){
    knockReadings[i]=0;
  }
  
  int currentKnockNumber=0;         			// Incrementer for the array.
  int startTime=millis();           			// Reference for when this knock started.
  int now=millis();
  
  delay(knockFadeTime);                       	        // wait for this peak to fade before we listen to the next one.

  do {
    //listen for the next knock or wait for it to timeout. 
    int knockSensorValue = analogRead(knockSensor);
    if (knockSensorValue >= threshold){                   //got another knock...
      //record the delay time.
      Serial.println("knock.");
      now=millis();
      knockReadings[currentKnockNumber] = now-startTime;
      currentKnockNumber++;                             //increment the counter
      startTime=now;          
      // and reset our timer for the next knock
      delay(knockFadeTime);                              // again, a little delay to let the knock decay.
    }

    now=millis();
    
    //did we timeout or run out of knocks?
  } while ((now-startTime < knockComplete) && (currentKnockNumber < maximumKnocks));
  
  //we've got our knock recorded, lets see if it's valid
  if (validateKnock() == true){
    turnRelayOn();
  } else {
    Serial.println("Secret knock failed.");
    // We didn't unlock, so blink the signal LED as visual feedback.
    for (i=0; i<3; i++){					
      digitalWrite(signalLed, HIGH);
      delay(100);
      digitalWrite(signalLed, LOW);
      delay(100);
    }
  }
}

// Sees if our knock matches the secret.
// returns true if it's a good knock, false if it's not.
// todo: break it into smaller functions for readability.
boolean validateKnock(){
  int i=0;
 
  // simplest check first: Did we get the right number of knocks?
  int currentKnockCount = 0;
  int maxKnockInterval = 0;          			// We use this later to normalize the times.
  
  for (i=0; i<maximumKnocks; i++){
    if (knockReadings[i] > 0){
      currentKnockCount++;
    }
    
    if (knockReadings[i] > maxKnockInterval){ 	// collect normalization data while we're looping.
      maxKnockInterval = knockReadings[i];
    }
  }
  
  for (i=0; i<maximumKnocks; i++){
    knockReadings[i]= map(knockReadings[i],0, maxKnockInterval, 0, 100); // Normalize the times
    Serial.println(knockReadings[i]);
  }
  
  if (currentKnockCount != secretKnockCount){
    return false; 
  }
  
  /*  Now we compare the relative intervals of our knocks, not the absolute time between them.
      (ie: if you do the same pattern slow or fast it should still open the door.)
      This makes it less picky, which while making it less secure can also make it
      less of a pain to use if you're tempo is a little slow or fast. 
  */
  int totaltimeDifferences=0;
  int timeDiff=0;
  for (i=0; i<secretKnockCount; i++){
    timeDiff = abs(knockReadings[i]-secretCode[i]);
    if (timeDiff > rejectValue){ // Individual value too far out of whack
      return false;
    }
    totaltimeDifferences += timeDiff;
  }
  
  // It can also fail if the whole thing is too inaccurate.
  if (totaltimeDifferences/secretKnockCount>averageRejectValue){
    return false; 
  }
  
  return true;
}

