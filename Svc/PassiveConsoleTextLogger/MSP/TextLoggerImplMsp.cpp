#include <Svc/TextLogger/TextLoggerImpl.hpp>
#include <Fw/Types/BasicTypes.hpp>
#include <Fw/Types/Assert.hpp>
#include <stdio.h>
#include <string.h>
// MSP430 headers
extern "C" {
 #include "msp430.h"
 #include <TiHal/hal_lcd.h>
}

#define USE_USB_UART 0
#define LINE_WRAP 8
#define LINE_SIZE 16

void ledDebug(bool on);
namespace Svc {
    
#if USE_USB_UART        
    void writeToSer(const char* str) {
        unsigned int i, len;
       
        len = strlen(str);
       
        for(i=0 ; i<len ; i++)
        {
          while(!(UCA1IFG&UCTXIFG));
          UCA1TXBUF = (unsigned char) str[i];
        }        
    }
#endif
    
    void ConsoleTextLoggerImpl::init(void) {
        PassiveTextLoggerComponentBase::init();
        //  ledDebug(true);
        // Initialize display hardware
        halLcdInit();
        halLcdBackLightInit();
        halLcdSetBackLight(16);
        halLcdSetContrast(100);
        halLcdClearScreen();

        halLcdPrintLine("++++ ISF Log ++++",0,OVERWRITE_TEXT);
        halLcdPrintLine("-----------------",1,OVERWRITE_TEXT);
        
#if USE_USB_UART        
        P5SEL |= BIT6 + BIT7;                     // P5.6,7 = USCI_A1 TXD/RXD
        UCA1CTL1 |= UCSWRST;                      // **Put state machine in reset**
        UCA1CTL1 |= UCSSEL__ACLK;                 // AMCLK
        UCA1BR0 = 27;                             // 32,768kHz 1200 (see User's Guide)
        UCA1BR1 = 0;                              // 32,768kHz 1200
        UCA1MCTL = UCBRS_2;                       // 32,768kHz 1200
        UCA1CTL1 &= ~UCSWRST; 
        
        writeToSer("Test Logger created.\n");
#endif        
        
    }
    
    void ConsoleTextLoggerImpl::TextLogger_handler(NATIVE_INT_TYPE portNum, U32 id, Fw::Time &timeTag, Fw::TextLogSeverity severity, Fw::TextLogString &text) {
#if USE_USB_UART        
        writeToSer(msg.toChar());
#endif
        // Erase lines ahead
        if (LINE_WRAP == this->m_displayLine) {
            halLcdPrintLine("                 ",2,OVERWRITE_TEXT);
        } else {
            halLcdPrintLine("                 ",this->m_displayLine+1,OVERWRITE_TEXT);
        }
        // erase current line
        halLcdPrintLine("                ",this->m_displayLine,OVERWRITE_TEXT);
        // truncate current line
        char line[LINE_SIZE+1];
        memcpy(line,msg.toChar(),LINE_SIZE);
        line[LINE_SIZE] = 0;
        // write current line
        halLcdPrintLine(line,this->m_displayLine,OVERWRITE_TEXT);
        if (LINE_WRAP == this->m_displayLine) {
            this->m_displayLine = 2; 
        } else {
            this->m_displayLine++;
        }
        
    }

}
