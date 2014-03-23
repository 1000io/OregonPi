/*
  RCSwitch - Arduino libary for remote control outlet switches
  Copyright (c) 2011 Suat �zg�r.  All right reserved.

  * Largely modified and split by disk91 - http://www.disk91.com

  Contributors:
  - Andre Koehler / info(at)tomate-online(dot)de
  - Gordeev Andrey Vladimirovich / gordeev(at)openpyro(dot)com
  
  Project home: http://code.google.com/p/rc-switch/

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/
#ifndef _RCSwitch_h
#define _RCSwitch_h

#if defined(ARDUINO) && ARDUINO >= 100
    #include "Arduino.h"
#else
    #include <wiringPi.h>
    #include <stdint.h>
// -- coment by disk91    #define NULL 0
    #define CHANGE 1
#ifdef __cplusplus
extern "C"{
#endif
typedef uint8_t boolean;
typedef uint8_t byte;

#if !defined(NULL)
#endif
#ifdef __cplusplus
}
#endif
#endif


// Number of maximum High/Low changes per packet.
// We can handle up to (unsigned long) => 32 bit * 2 H/L changes per bit + 2 for sync
#define RCSWITCH_MAX_CHANGES 67

// Taille max d'un message de type Oregon Scientific
#define RCSWITCH_MAX_MESS_SIZE 128

class RCSwitch {

  public:
    RCSwitch(int rxpin,int txpin);
    

    void sendTriState(char* Code);
    void send(unsigned long Code, unsigned int length);
    void send(char* Code);
    
    void enableReceive(int interrupt);
    void enableReceive();
    void disableReceive();
	
    void enableTransmit(int nTransmitterPin);
    void disableTransmit();
  
    static bool OokAvailable();
    static bool getOokCode(char * _dest);
    static void OokResetAvailable();
    void transmit(int nHighPulses, int nLowPulses);

  private:

    static void handleInterrupt();
    int nReceiverInterrupt;
    int nTransmitterPin;

    static char OokReceivedCode[RCSWITCH_MAX_MESS_SIZE];
    static bool OokAvailableCode;
    
};

#endif
