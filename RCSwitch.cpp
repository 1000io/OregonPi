/*
  RCSwitch - Arduino libary for remote control outlet switches
  Copyright (c) 2011 Suat Özgür.  All right reserved.
  
  * Largely modified and split by disk91 - http://www.disk91.com

  Contributors:
  - Andre Koehler / info(at)tomate-online(dot)de
  - Gordeev Andrey Vladimirovich / gordeev(at)openpyro(dot)com
  - Skineffect / http://forum.ardumote.com/viewtopic.php?f=2&t=48
  
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

#include <stdio.h>
#include <string.h>

#include "RCSwitch.h"
#include "RcOok.h"

char RCSwitch::OokReceivedCode[RCSWITCH_MAX_MESS_SIZE];
bool RCSwitch::OokAvailableCode;

OregonDecoderV2 orscV2;
OregonDecoderV3 orscV3;
RCSwitch_ rcswp1;
//CrestaDecoder cres;
//KakuDecoder kaku;
//XrfDecoder xrf;
//HezDecoder hez;
//VisonicDecoder viso;
//EMxDecoder emx;
//KSxDecoder ksx;
//FSxDecoder fsx;

/* =================================================
 * Construct RCSwitch
 */
RCSwitch::RCSwitch(int rxpin, int txpin) {
	RCSwitch::OokAvailableCode = false;
	RCSwitch::OokReceivedCode[0] = '\0';
	rcswp1.configure(1,this);

	if (rxpin != -1 ) {
		this->enableReceive(rxpin);
	} else this->nReceiverInterrupt = -1;

	if (txpin != -1 ) {
		this->enableTransmit(txpin);
	} else this->nTransmitterPin = -1;
}

/**
 * Enable transmissions
 *
 * @param nTransmitterPin    Arduino Pin to which the sender is connected to
 */
void RCSwitch::enableTransmit(int nTransmitterPin) {
  this->nTransmitterPin = nTransmitterPin;
  pinMode(this->nTransmitterPin, OUTPUT);
  digitalWrite(this->nTransmitterPin, LOW);
}

/**
  * Disable transmissions
  */
void RCSwitch::disableTransmit() {
  this->nTransmitterPin = -1;
}

/**
 * Enable receiving data
 */
void RCSwitch::enableReceive(int interrupt) {
  this->nReceiverInterrupt = interrupt;
  this->enableReceive();
}

void RCSwitch::enableReceive() {
  if (this->nReceiverInterrupt != -1) {
    wiringPiISR(this->nReceiverInterrupt, INT_EDGE_BOTH, &handleInterrupt);
  }
}

/**
 * Disable receiving data
 */
void RCSwitch::disableReceive() {
  this->nReceiverInterrupt = -1;
}

// ==============================================
// Set to true when a code has been decode by the
// OoK modul
bool RCSwitch::OokAvailable() {
	return RCSwitch::OokAvailableCode;
}

// ==============================================
// Return the received code decoded by Ook engine
// if available and true, otherwith return false
// can be used w/o OokAvailable
//
// The decoded value is stored in the v this string
// must have a size equal to RCSWITCH_MAX_MESS_SIZE
//
// Reset available flag (this allow new capture from
// interrupt (locked otherwize to avoid reentrance

bool RCSwitch::getOokCode(char * _dest) {
	if ( RCSwitch::OokAvailableCode ) {
		strcpy(_dest,RCSwitch::OokReceivedCode);
		RCSwitch::OokAvailableCode = false;
		return true;
	} else return false;
}

// =============================================
// reset available (autorize new capture)
void RCSwitch::OokResetAvailable() {
	RCSwitch::OokAvailableCode = false;
}

// ==============================================
// Interrupt Handler to manage the different protocols
void RCSwitch::handleInterrupt() {

  static unsigned int duration;
  static unsigned long lastTime;

  long time = micros();
  duration = time - lastTime;
  lastTime = time;
  word p = (unsigned short int) duration;

  // Avoid re-entry
  if ( !OokAvailableCode ) {		// avoid reentrance -- wait until data is read
	  if (orscV2.nextPulse(p)) 	{ RCSwitch::OokAvailableCode = true; orscV2.sprint("OSV2 ",RCSwitch::OokReceivedCode); orscV2.resetDecoder(); }
	  if (orscV3.nextPulse(p)) 	{ RCSwitch::OokAvailableCode = true; orscV3.sprint("OSV3 ",RCSwitch::OokReceivedCode); orscV3.resetDecoder(); }
	  if (rcswp1.nextPulse(p)) 	{ RCSwitch::OokAvailableCode = true; rcswp1.sprint("ALRM ",RCSwitch::OokReceivedCode); rcswp1.resetDecoder(); }

	//  if (cres.nextPulse(p)) 	{ cres.print("CRES"); cres.resetDecoder(); }
	//  if (kaku.nextPulse(p)) 	{ kaku.print("KAKU"); kaku.resetDecoder(); }
	//  if (xrf.nextPulse(p))  	{ xrf.print("XRF"); xrf.resetDecoder(); }
	//  if (hez.nextPulse(p)) 	{ hez.print("HEZ"); hez.resetDecoder(); }
	//  if (viso.nextPulse(p)) 	{ viso.print("VISO"); viso.resetDecoder(); }
	//  if (emx.nextPulse(p)) 	{ emx.print("EMX"); emx.resetDecoder(); }
	//  if (ksx.nextPulse(p)) 	{ ksx.print("KSX"); ksx.resetDecoder(); }
	//  if (fsx.nextPulse(p)) 	{ fsx.print("FSX"); fsx.resetDecoder(); }
  }
}

// =======================================================
// Transmit pulse
void RCSwitch::transmit(int nHighPulses, int nLowPulses) {
    boolean disabled_Receive = false;
    //XXX renzo38 int nReceiverInterrupt_backup = nReceiverInterrupt;
    if (this->nTransmitterPin != -1) {
        if (this->nReceiverInterrupt != -1) {
// XXX voir si on active ou non la reception lors de la tansmission ???
// disk           this->disableReceive();
            disabled_Receive = true;
        }
        digitalWrite(this->nTransmitterPin, HIGH);
        delayMicroseconds( nHighPulses);
        digitalWrite(this->nTransmitterPin, LOW);
        delayMicroseconds( nLowPulses);
        if(disabled_Receive){
// XXX disk           this->enableReceive(nReceiverInterrupt_backup);
        }
    }
}
