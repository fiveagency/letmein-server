/*
  Secret knock sender.
  Detects a pattern of knocks and sends a signal to receiver if pattern is correct.

  Used part of the code of Secret knock:
  By Steve Hoefer http://grathio.com
  Version 0.1.10.20.10
  Licensed under Creative Commons Attribution-Noncommercial-Share Alike 3.0
  http://creativecommons.org/licenses/by-nc-sa/3.0/us/
  (In short: Do what you want, just be sure to include this line and the four above it, and don't sell it or use it in anything you sell without contacting me.)

  Used part of the code from Demo Application for MSP430 eZ430-RF2500 / CC1100-2500 Interface:
*/
/* --COPYRIGHT--,BSD
* Copyright (c) 2011, Texas Instruments Incorporated
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions
* are met:
*
* *  Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
*
* *  Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the
*    documentation and/or other materials provided with the distribution.
*
* *  Neither the name of Texas Instruments Incorporated nor the names of
*    its contributors may be used to endorse or promote products derived
*    from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
* THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
* PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
* CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
* EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
* PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
* OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
* WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
* OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
* EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
* --/COPYRIGHT--*/

#include "msp430x22x4.h"
#include "include.h"

#define maximumKnocks 10 // Maximum number of knocks to listen for.

const int threshold = 0x14;         // Minimum signal from the piezo to register as a knock
const int analogReadDelay = 12000;  // MCLK cycles we allow a knock to fade before we listen for another one. (Debounce)
const int knockIgnoreTime = 1000;       // we ignore knocks that are closer to previous knock than this time (Debounce)
const int knockComplete = 14400;    // TAR counts to wait for a knock before we assume that it's finished. Equals 1200 miliseconds
const int rejectValue = 25;         // If an individual knock is off by this percentage of a knock we don't unlock..
const int averageRejectValue = 15;  // If the average timing of the knocks is off by this percent we don't unlock.

const int secretCode[maximumKnocks] = {50, 100, 100, 0, 0, 0, 0, 0, 0, 0};  // Initial setup: two short, two long
//const int secretCode[maximumKnocks] = {50, 25, 25, 50, 100, 50, 0, 0, 0, 0};  // Initial setup: "Shave and a Hair Cut, two bits."
//const int secretCode[maximumKnocks] = {100, 100, 0, 0, 0, 0, 0, 0, 0, 0};
int secretKnockCount = 0; // number of elements in secretCode that are greater then 0

int knockReadings[maximumKnocks];   // When someone knocks this array fills with delays between knocks.

extern char paTable[];
extern char paTableLen;
char txBuffer[4];
const int packageContent = 0x34;

void setup() {
    int i;
    
    WDTCTL = WDTPW + WDTHOLD; // Stop Watch Dog Timer
    
    // 5ms delay to compensate for time to startup between MSP430 and CC1100/2500
    __delay_cycles(5000);
    TI_CC_SPISetup(); // Initialize SPI port
    
    P2SEL = 0;                                // Sets P2.6 & P2.7 as GPIO
    TI_CC_PowerupResetCCxxxx();               // Reset CCxxxx
    writeRFSettings();                        // Write RF settings to config reg
    TI_CC_SPIWriteBurstReg(TI_CCxxx0_PATABLE, paTable, paTableLen);//Write PATABLE
    
    TI_CC_GDO0_PxIES |= TI_CC_GDO0_PIN;       // Int on falling edge (end of pkt)
    TI_CC_GDO0_PxIFG &= ~TI_CC_GDO0_PIN;      // Clear flag
    TI_CC_GDO0_PxIE |= TI_CC_GDO0_PIN;        // Enable int on end of packet
    
    BCSCTL3 |= LFXT1S_2; // Set LFXT1 to the VLO @ 12kHz, used by Timer A
    
    // LEDs
    P1DIR = BIT0 + BIT1; // P1.0 and P1.1 output
    P1OUT &= ~(BIT0 + BIT1); // P1.0 and P1.1 output LOW, LEDs Off
    
    // ADC setup
    ADC10CTL0 = ADC10SHT_2 + ADC10ON + ADC10IE; // sample-and-hold time, ADC10ON, interrupt enabled
    ADC10AE0 |= BIT0; // P2.0 is analog input
    
    // precalculate number of knocks in secret knock sequence
    for (i = 0; i < maximumKnocks; i++) {
        if (secretCode[i] > 0) {
            secretKnockCount++;
        }
    }
}

int analogRead() {
    int adcValue;
    ADC10CTL0 |= ENC + ADC10SC; // Sampling and conversion start
    __bis_SR_register(CPUOFF + GIE); // LPM0, ADC10_ISR will force exit
    adcValue = ADC10MEM;
    TI_CC_Wait(analogReadDelay);
    return adcValue;
}

void startTimer() {
    TACTL |= TASSEL_1 + MC_2 + ID_0; // Set Timer source to ACLK, Continous up and no divider
}

void stopAndResetTimer() {
    TACTL |= TACLR;
}

void normalizeKnockReadings() {
    int i = 0;
    int maxKnockInterval = 0; // We use this later to normalize the times.
    
    for (i = 0; i < maximumKnocks; i++) {
        if (knockReadings[i] > maxKnockInterval) {
            maxKnockInterval = knockReadings[i];
        }
    }
    
    if (maxKnockInterval > 0) {
        for (i = 0; i < maximumKnocks; i++) {
            int knockReading = knockReadings[i];
            knockReading = (int)(((float)knockReading / maxKnockInterval) * 100);
            knockReadings[i] = knockReading;
        }
    }
}

int validateKnock() {
    int i = 0;
    int totaltimeDifferences=0;
    int timeDiff=0;
    int currentKnockCount = 0;
    
    for (i = 0; i < maximumKnocks; i++) {
        if (knockReadings[i] > 0){
            currentKnockCount++;
        }
    }
    
    // simplest check first: Did we get the right number of knocks?
    if (currentKnockCount != secretKnockCount){
        return 0; 
    }
  
    normalizeKnockReadings();
    
    for (i = 0; i < secretKnockCount; i++) {
        timeDiff = abs(knockReadings[i] - secretCode[i]);
        if (timeDiff > rejectValue){ // Individual value too far out of whack
            return 0;
        }
        totaltimeDifferences += timeDiff;
    }
    
    // It can also fail if the whole thing is too inaccurate.
    if (totaltimeDifferences / secretKnockCount > averageRejectValue) {
        return 0; 
    }
  
    return 1;
}

// Records the timing of knocks.
void listenToSecretKnock() {
    int i = 0;
    int currentKnockNumber = 0; // Incrementer for the array.
    unsigned int lastKnockTime = TAR; // Reference for when previous knock started.
    int isValid = 0;
    
    // First lets reset the listening array.
    for (i = 0; i < maximumKnocks; i++) {
        knockReadings[i] = 0;
    }
  
    do {
        int knockSensorValue = analogRead();
        if (knockSensorValue > threshold) { //got another knock...
            // record the delay time.
            int timeDiff = TAR - lastKnockTime;
            if (timeDiff > knockIgnoreTime) {
                // ignore everything under knockIgnoreTime timer counts
                knockReadings[currentKnockNumber] = TAR - lastKnockTime;
                currentKnockNumber++;
                lastKnockTime = TAR;
            }
        }
        
      // did we timeout or run out of knocks?
    } while((TAR - lastKnockTime < knockComplete) && (currentKnockNumber < maximumKnocks));
    
    isValid = validateKnock();
    if (isValid == 1) {
        // Knock is correct 
        
        // Build packet
        txBuffer[0] = 2;                        // Packet length
        txBuffer[1] = 0x01;                     // Packet address
        txBuffer[2] = packageContent;           // Packet content
        RFSendPacket(txBuffer, 3);              // Send value over RF
    
        // blink the green LED
        P1OUT ^= BIT1;
        TI_CC_Wait(50000);
        P1OUT ^= BIT1;
    }
    else {
        // We didn't unlock, so blink the red LED
        P1OUT ^= BIT0;
        TI_CC_Wait(50000);
        P1OUT ^= BIT0;
    }
}

void main(void) {
    setup();
    
    while(1) {
        int knockSensorValue = analogRead();
        if (knockSensorValue > threshold) {
            startTimer();
            listenToSecretKnock();
            stopAndResetTimer();
        }
    }
}

// ADC10 interrupt service routine
#pragma vector=ADC10_VECTOR
__interrupt void ADC10_ISR(void) {
    __bic_SR_register_on_exit(CPUOFF);  // Clear CPUOFF bit from 0(SR), exit Low Power Mode 0
}

// The ISR assumes the interrupt came from GDO0. GDO0 fires indicating that
// CCxxxx received a packet
#pragma vector=PORT2_VECTOR
__interrupt void Port2_ISR(void)
{
  TI_CC_GDO0_PxIFG &= ~TI_CC_GDO0_PIN;      // After pkt RX, this flag is set.
}
