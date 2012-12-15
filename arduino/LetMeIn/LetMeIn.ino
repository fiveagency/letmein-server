/* 
  LetMeIn door opener. When OPEN command is received on serial port door is opened.
  eZ430-RF2500T wireless board is connected to digital input, when that digital in is high door should be opened.
  ENC28J60 ethernet module is connected to arduino and is listening for http requests that match GET /door/open. If that GET request is received door is opened
 */
 
#include <EtherCard.h>

#define STATIC 1  // set to 1 to disable DHCP (adjust myip/gwip values below)

const int RELAY_ON = LOW;
const int RELAY_OFF = HIGH;
const int RELAY_SIGNAL_DURATION = 250;
const int relay = 4;
const int MSP430 = 2;

int state = RELAY_OFF;
unsigned long relaySignalStartMilis = 0;

#if STATIC
// ethernet interface ip address
static byte myip[] = { 10,5,1,230 };
// gateway ip address
static byte gwip[] = { 10,5,1,1 };
#endif

// ethernet mac address - must be unique on your network
static byte MAC[] = { 0x74,0x69,0x69,0x2D,0x30,0x38 };

byte Ethernet::buffer[500]; // tcp/ip send and receive buffer
char replyOK[] PROGMEM = "HTTP/1.1 200 OK\r\n";
char replyUnauthorized[] PROGMEM = "HTTP/1.1 401 Unauthorized\r\n";
char replyBadRequest[] PROGMEM = "HTTP/1.1 400 Bad Request\r\n";

String PIN = "1234";

void setup() {
  digitalWrite(relay, RELAY_OFF);
  pinMode(relay, OUTPUT);
  
  Serial.begin(9600);
  Serial.println("Program start.");
  
  MSP430Setup();
  ethernetSetup();
}

void MSP430Setup() {
  pinMode(MSP430, INPUT);
}

void ethernetSetup() {
  if (ether.begin(sizeof Ethernet::buffer, MAC) == 0) {
    Serial.println( "Failed to access Ethernet controller");
  }
#if STATIC
  ether.staticSetup(myip, gwip);
#else
  if (!ether.dhcpSetup())
    Serial.println("DHCP failed");
#endif

  Serial.print("MAC: ");
  for (byte i = 0; i < 6; ++i) {
    Serial.print(MAC[i], HEX);
    if (i < 5)
      Serial.print(':');
  }
  Serial.println();
  
  ether.printIp("IP:  ", ether.myip);
  ether.printIp("GW:  ", ether.gwip);  
  ether.printIp("DNS: ", ether.dnsip); 
}

void loop(){
  turnRelayOff();
  checkMSP430();
  checkEthernet();
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

void checkEthernet() {
  // DHCP expiration is a bit brutal, because all other ethernet activity and
  // incoming packets will be ignored until a new lease has been acquired
  if (!STATIC && !ether.dhcpValid()) {
    Serial.println("Acquiring DHCP lease again");
    ether.dhcpSetup();
  }

  word len = ether.packetReceive();
  word pos = ether.packetLoop(len);
  if (pos) {
    String data = (char *) Ethernet::buffer + pos;

    boolean openCommand = data.startsWith("POST /door/open HTTP");
    boolean validateCommand = data.startsWith("POST /door/validate HTTP");
    boolean correctPin = data.indexOf("pin=" + PIN) > -1;
    
    boolean changePinCommand = data.startsWith("POST /door/changePin HTTP");
    boolean correctOldPin = data.indexOf("oldPin=" + PIN) > -1;

    if(correctPin && (openCommand || validateCommand)) {
      if (openCommand) {
        Serial.println("Received OPEN command via Ethernet");
        turnRelayOn();
      }
      if (validateCommand) {
        Serial.println("Validated pin via Ethernet");
      }
      memcpy_P(ether.tcpOffset(), replyOK, sizeof replyOK);
      ether.httpServerReply(sizeof replyOK - 1);
    }
    else if (changePinCommand && correctOldPin) {
      int start = data.indexOf("newPin=");
      if (start > -1) {
        String newPin = data.substring(start + 7, start + 7 + 4);
        if (newPin.length() == 4) {
          Serial.println(newPin);
          PIN = newPin;
          memcpy_P(ether.tcpOffset(), replyOK, sizeof replyOK);
          ether.httpServerReply(sizeof replyOK - 1);
        }
        else {
          memcpy_P(ether.tcpOffset(), replyBadRequest, sizeof replyBadRequest);
          ether.httpServerReply(sizeof replyBadRequest - 1);
        }
      }
      else {
        memcpy_P(ether.tcpOffset(), replyBadRequest, sizeof replyBadRequest);
        ether.httpServerReply(sizeof replyBadRequest - 1);
      }
    }
    else {
      memcpy_P(ether.tcpOffset(), replyUnauthorized, sizeof replyUnauthorized);
      ether.httpServerReply(sizeof replyUnauthorized - 1);
    }
  }
}

