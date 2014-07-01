const int knockSensor = A0;

// Tuning constants.  Could be made vars and hoooked to potentiometers for soft configuration, etc.
const int threshold = 300;           // Minimum signal from the piezo to register as a knock
const int rejectValue = 25;        // If an individual knock is off by this percentage of a knock we don't unlock..
const int averageRejectValue = 15; // If the average timing of the knocks is off by this percent we don't unlock.
const int knockFadeTime = 150;     // milliseconds we allow a knock to fade before we listen for another one. (Debounce timer.)

const int maximumKnocks = 10;       // Maximum number of knocks to listen for.
const int knockComplete = 1200;     // Longest time to wait for a knock before we assume that it's finished.

const int outputPin = 4; // set this pin to high when knock succeeds
int RXLED = 17;

//int secretCode[maximumKnocks] = {50, 100, 100, 0, 0, 0, 0, 0, 0, 0};  // Initial setup: two short, two long
int secretCode[maximumKnocks] = {50, 25, 25, 50, 100, 50, 0, 0, 0, 0};  // Initial setup: "Shave and a Hair Cut, two bits."

int knockReadings[maximumKnocks];   // When someone knocks this array fills with delays between knocks.
int secretKnockCount = 0;

void setup() {
  
  Serial.begin(9600);
  Serial.println("Program start.");
  
  knockSetup();
  pinMode(outputPin, OUTPUT);
  pinMode(RXLED, OUTPUT);  // Set RX LED as an output
}

void knockSetup() {
  for (int i=0; i<maximumKnocks; i++){
    if (secretCode[i] > 0) {  	//todo: precalculate this.
      secretKnockCount++;
    }
  }
}

void loop(){
  listenForKnock();
}

void listenForKnock() {
  // Listen for any knock at all.
  int knockSensorValue = analogRead(knockSensor);
  
  if (knockSensorValue >= threshold){
    listenToSecretKnock();
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
  if (validateKnock() == true) {
    digitalWrite(outputPin, HIGH);
    delay(10);
    digitalWrite(outputPin, LOW);
    Serial.println("Secret knock succeeded.");
    digitalWrite(RXLED, LOW);   // set the LED on
    delay(500);
    digitalWrite(RXLED, HIGH);    // set the LED off
    delay(500);
    digitalWrite(RXLED, LOW);   // set the LED on
    delay(500);
    digitalWrite(RXLED, HIGH);    // set the LED off
  } else {
    Serial.println("Secret knock failed.");
    digitalWrite(RXLED, LOW);   // set the LED on
    delay(100);
    digitalWrite(RXLED, HIGH);    // set the LED off
    delay(100);
    digitalWrite(RXLED, LOW);   // set the LED on
    delay(100);
    digitalWrite(RXLED, HIGH);    // set the LED off
    delay(100);
    digitalWrite(RXLED, LOW);   // set the LED on
    delay(100);
    digitalWrite(RXLED, HIGH);    // set the LED off
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