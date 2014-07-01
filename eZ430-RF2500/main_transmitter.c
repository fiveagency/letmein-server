/*
  Secret knock transmitter.

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

#include "include.h"

extern char paTable[];
extern char paTableLen;
char txBuffer[4];
const int packageContent = 0x34;

void setup() {

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

    // Build packet
	txBuffer[0] = 2;                        // Packet length
	txBuffer[1] = 0x01;                     // Packet address
	txBuffer[2] = packageContent;           // Packet content

    // LED
    P1DIR = BIT1; // P1.1 output
    P1OUT &= ~BIT1; // P1.1 output LOW, LED Off

    P2IE |= BIT0;                             // P2.0 interrupt enabled
    P2IES |= BIT0;                            // P2.0 Hi/lo edge
    P2IFG &= ~BIT0;                           // P2.0 IFG cleared
}

void main(void) {
    setup();

    __bis_SR_register(LPM4_bits + GIE);       // Enter LPM4 w/interrupt
}

// The ISR assumes the interrupt came from GDO0. GDO0 fires indicating that
// CCxxxx received a packet
#pragma vector=PORT2_VECTOR
__interrupt void Port2_ISR(void)
{
	RFSendPacket(txBuffer, 3);              // Send value over RF

	// blink the green LED
	P1OUT ^= BIT1;
	TI_CC_Wait(50000);
	P1OUT ^= BIT1;
	TI_CC_Wait(50000);

	P2IFG &= ~BIT0;                           // P2.0 IFG cleared
}
