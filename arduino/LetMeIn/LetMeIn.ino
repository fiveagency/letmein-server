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
static byte mymac[] = { 0x74,0x69,0x69,0x2D,0x30,0x38 };

byte Ethernet::buffer[500]; // tcp/ip send and receive buffer
char pageOK[] PROGMEM = "HTTP/1.1 200 OK\r\n";
char pageFail[] PROGMEM = "HTTP/1.1 401 Unauthorized\r\n";

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
  if (ether.begin(sizeof Ethernet::buffer, mymac) == 0) {
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
    Serial.print(mymac[i], HEX);
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
    char* data = (char *) Ethernet::buffer + pos;
    
    if(strstr(data, "GET /door/open") != 0) {
      Serial.println("Received OPEN command via Ethernet");
      turnRelayOn();
      memcpy_P(ether.tcpOffset(), pageOK, sizeof pageOK);
      ether.httpServerReply(sizeof pageOK - 1);
    }
    else {
      memcpy_P(ether.tcpOffset(), pageFail, sizeof pageFail);
      ether.httpServerReply(sizeof pageFail - 1);
    }
  }
}

