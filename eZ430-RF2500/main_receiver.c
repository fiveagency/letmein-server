/*
  Secret knock reciever.
  Listens for signal from sender and raises P4.3 when signal is received

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
char rxBuffer[4];
unsigned int i = 0;

const int packageContent = 0x34;

void main (void)
{
    WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT

    // 5ms delay to compensate for time to startup between MSP430 and CC1100/2500
    __delay_cycles(5000);
  
    TI_CC_SPISetup();                         // Initialize SPI port

    P2SEL = 0;                                // Sets P2.6 & P2.7 as GPIO
    TI_CC_PowerupResetCCxxxx();               // Reset CCxxxx
    writeRFSettings();                        // Write RF settings to config reg
    TI_CC_SPIWriteBurstReg(TI_CCxxx0_PATABLE, paTable, paTableLen);//Write PATABLE

    // Configure ports -- switch inputs, LEDs, GDO0 to RX packet info from CCxxxx
    TI_CC_LED_PxOUT &= ~(TI_CC_LED1 + TI_CC_LED2); // Outputs = 0
    TI_CC_LED_PxDIR |= TI_CC_LED1 + TI_CC_LED2;// LED Direction to Outputs

    TI_CC_GDO0_PxIES |= TI_CC_GDO0_PIN;       // Int on falling edge (end of pkt)
    TI_CC_GDO0_PxIFG &= ~TI_CC_GDO0_PIN;      // Clear flag
    TI_CC_GDO0_PxIE |= TI_CC_GDO0_PIN;        // Enable int on end of packet

    TI_CC_SPIStrobe(TI_CCxxx0_SRX);         // Initialize CCxxxx in RX mode.
                                            // When a pkt is received, it will
                                            // signal on GDO0 and wake CPU

    P4DIR = BIT3; // P4.3 is output
    P4OUT &= ~BIT3;
    
  __bis_SR_register(LPM3_bits + GIE);       // Enter LPM3, enable interrupts
}


// The ISR assumes the interrupt came from GDO0. GDO0 fires indicating that
// CCxxxx received a packet
#pragma vector=PORT2_VECTOR
__interrupt void Port2_ISR(void)
{
    // if GDO fired
    if(TI_CC_GDO0_PxIFG & TI_CC_GDO0_PIN)
    {
      char len=2;                           // Len of pkt to be RXed (only addr
                                            // plus data; size byte not incl b/c
                                            // stripped away within RX function)
      if (RFReceivePacket(rxBuffer,&len)) { // Fetch packet from CCxxxx
          int rxValue = rxBuffer[1];
          if (rxValue == packageContent) {
              P4OUT ^= BIT3;
              TI_CC_LED_PxOUT ^= BIT0;
              __delay_cycles(100);
              P4OUT ^= BIT3;
              TI_CC_LED_PxOUT ^= BIT0;
          }
      }
    }

    TI_CC_GDO0_PxIFG &= ~TI_CC_GDO0_PIN;      // After pkt RX, this flag is set.
}
