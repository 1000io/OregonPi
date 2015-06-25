/* ===================================================
 * RcOok.cpp
 * ---------------------------------------------------
 * 433 Mhz decoding OoK frame from Oregon Scientific
 *
 *  Created on: 16 sept. 2013
 *  Author: disk91 ( http://www.disk91.com ) modified from
 *   Oregon V2 decoder added - Dominique Pierre
 *   Oregon V3 decoder revisited - Dominique Pierre
 *   RwSwitch :  Copyright (c) 2011 Suat Özgür.  All right reserved.
 *     Contributors:
 *        - Andre Koehler / info(at)tomate-online(dot)de
 *        - Gordeev Andrey Vladimirovich / gordeev(at)openpyro(dot)com
 *        - Skineffect / http://forum.ardumote.com/viewtopic.php?f=2&t=48
 *        Project home: http://code.google.com/p/rc-switch/
 *
 *   New code to decode OOK signals from weather sensors, etc.
 *   2010-04-11 <jcw@equi4.com> http://opensource.org/licenses/mit-license.php
 *	 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * 	 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * 	 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * 	 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *	 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * 	 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * 	 THE SOFTWARE.
 * ===================================================
 */
#include <iostream>

#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "RCSwitch.h"
#include "RcOok.h"

//#define TRACE_RCOOK

/* ======================================================
 * Master class OOK
 * ------------------------------------------------------
 */

    DecodeOOK::DecodeOOK () { resetDecoder(); }

    bool DecodeOOK::nextPulse (word width) {
        if (state != DONE)

            switch (decode(width)) {
                case -1: resetDecoder(); break;
                case 1:  done(); break;
            }
        return isDone();
    }

    bool DecodeOOK::isDone() const { return state == DONE; }

    const byte* DecodeOOK::getData (byte& count) const {
        count = pos;
        return data;
    }

    void DecodeOOK::resetDecoder () {
        total_bits = bits = pos = flip = 0;
        state = UNKNOWN;
    }

    // add one bit to the packet data buffer

    void DecodeOOK::gotBit (char value) {
        total_bits++;
        byte *ptr = data + pos;
        *ptr = (*ptr >> 1) | (value << 7);

        if (++bits >= 8) {
            bits = 0;
            if (++pos >= sizeof data) {
                resetDecoder();
                return;
            }
        }
        state = OK;
    }

    // store a bit using Manchester encoding
    void DecodeOOK::manchester (char value) {
        flip ^= value; // manchester code, long pulse flips the bit
        gotBit(flip);
    }

    // move bits to the front so that all the bits are aligned to the end
    void DecodeOOK::alignTail (byte max) {
        // align bits
        if (bits != 0) {
            data[pos] >>= 8 - bits;
            for (byte i = 0; i < pos; ++i)
                data[i] = (data[i] >> bits) | (data[i+1] << (8 - bits));
            bits = 0;
        }
        // optionally shift bytes down if there are too many of 'em
        if (max > 0 && pos > max) {
            byte n = pos - max;
            pos = max;
            for (byte i = 0; i < pos; ++i)
                data[i] = data[i+n];
        }
    }

    /* Reverse bits order last becoming first */
    void DecodeOOK::reverseData () {
    	// reverse octets
    	int start = 0;
    	int end = pos-1;
    	while ( start < end ) {
    		byte b = data[start];
    		data[start] = data[end];
    		data[end] = b;
    		start++;
    		end--;
    	}
    }

    void DecodeOOK::reverseBits () {
        for (byte i = 0; i < pos; ++i) {
            byte b = data[i];
            for (byte j = 0; j < 8; ++j) {
                data[i] = (data[i] << 1) | (b & 1);
                b >>= 1;
            }
        }
    }

    void DecodeOOK::reverseNibbles () {
        for (byte i = 0; i < pos; ++i)
            data[i] = (data[i] << 4) | (data[i] >> 4);
    }

    void DecodeOOK::done () {
        while (bits)
            gotBit(0); // padding
        state = DONE;
    }

    /**
     * Print in hex the received value into d string adding s string header
     * d minimal size is : OOK_MAX_STR_LEN
     * s recommended size : 3 char
     */
    void DecodeOOK::sprint(const char * s, char * d) {
    	char v[] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
#ifdef TRACE_RCOOK
    	char * _d = d;
#endif
        byte pos;
        const byte* data = this->getData(pos);
        sprintf(d,"%s ",s);
        d+=strlen(s);
        for (byte i = 0; i < pos ; ++i) {
            sprintf(d,"%c",v[ data[i] >> 4 ]);d++;
            sprintf(d,"%c",v[ data[i] & 0x0F]);d++;
        }
	*d='\0';

#ifdef TRACE_RCOOK
        std::cout << " * DecodeOOK::sprint() - received [" << _d << "]" << std::endl;
#endif
    }

    void DecodeOOK::print(const char* s) {
    	char t[128];
    	this->sprint(s,t);
        printf("%s\n",t);
    }

/* ======================================================
 * OregonDecoderV2
 * ------------------------------------------------------
 */

	OregonDecoderV2::OregonDecoderV2() {}

    void OregonDecoderV2::gotBit (char value) {
        if(!(total_bits & 0x01))
        {
            data[pos] = (data[pos] >> 1) | (value ? 0x80 : 00);
        }
        total_bits++;
        pos = total_bits >> 4;
        if (pos >= sizeof data) {
            resetDecoder();
            return;
        }
        state = OK;
    }

    int OregonDecoderV2::decode (word width) {
        if (200 <= width && width < 1200) {
            byte w = width >= 700;
            switch (state) {
                case UNKNOWN:
                    if (w != 0) {
                        // Long pulse
                        ++flip;
                    } else if ( 24 <= flip ) {
                    	//BugFix : initialement on test 32b mais il semble que
                    	// tous n'en aient pas autant, en tout cas on constate
                    	// plus de message reçus avec 24 que 32 ; obligatoire pour THN132N

                        // Short pulse, start bit
                        flip = 0;
                        state = T0;
                    } else {
                      // Reset decoder
                        return -1;
                    }
                    break;
                case OK:
                    if (w == 0) {
                        // Short pulse
                        state = T0;
                    } else {
                        // Long pulse
                        manchester(1);
                    }
                    break;
                case T0:
                    if (w == 0) {
                      // Second short pulse
                        manchester(0);
                    } else {
                        // Reset decoder
                        return -1;
                    }
                    break;
            }
        } else {
        	// Dans le cas du THN132N on a seulement 136b
        	// donc si on depasse le timing et que l'on a 136b
        	// c'est sans doute qu'il s'agit de celui-ci
        	return ( total_bits == 136 )? 1 : -1;
        }
        return total_bits == 160 ? 1: 0 ;
    }

/* ======================================================
 * OregonDecoderV3
 * ------------------------------------------------------
 */
	OregonDecoderV3::OregonDecoderV3() {}

    // add one bit to the packet data buffer
    void OregonDecoderV3::gotBit (char value) {
        data[pos] = (data[pos] >> 1) | (value ? 0x80 : 00);
        total_bits++;
        pos = total_bits >> 3;
        if (pos >= sizeof data) {
            resetDecoder();
            return;
        }
        state = OK;
    }

    int OregonDecoderV3::decode (word width) {
        if (200 <= width && width < 1200) {
            byte w = width >= 700;
            switch (state) {
                case UNKNOWN:
                    if (w == 0)
                        ++flip;
                    else if (32 <= flip) {
                        flip = 1;
                        manchester(1);
                    } else
                        return -1;
                    break;
                case OK:
                    if (w == 0)
                        state = T0;
                    else
                        manchester(1);
                    break;
                case T0:
                    if (w == 0)
                        manchester(0);
                    else
                        return -1;
                    break;
            }
        } else {
            return -1;
        }
        return  total_bits == 80 ? 1: 0;
    }

/* ======================================================
 * CrestaDecoder
 * ------------------------------------------------------
 */
	CrestaDecoder::CrestaDecoder () {}

	int CrestaDecoder::decode (word width) {
        if (200 <= width && width < 1300) {
            byte w = width >= 750;
            switch (state) {
                case UNKNOWN:
                    if (w == 1)
                        ++flip;
                    else if (2 <= flip && flip <= 10)
                        state = T0;
                    else
                        return -1;
                    break;
                case OK:
                    if (w == 0)
                        state = T0;
                    else
                        gotBit(1);
                    break;
                case T0:
                    if (w == 0)
                        gotBit(0);
                    else
                        return -1;
                    break;
            }
        } else if (width >= 2500 && pos >= 7)
            return 1;
        else
            return -1;
        return 0;
    }

/* ======================================================
 * KakuDecoder
 * ------------------------------------------------------
 */
	KakuDecoder::KakuDecoder () {}

	int KakuDecoder::decode (word width) {
        if ( ( 180 <= width && width < 450) || (950 <= width && width < 1250) ) {
            byte w = width >= 700;
            switch (state) {
                case UNKNOWN:
                case OK:
                    if (w == 0)
                        state = T0;
                    else
                        return -1;
                    break;
                case T0:
                    if (w)
                        state = T1;
                    else
                        return -1;
                    break;
                case T1:
                    state += w + 1;
                    break;
                case T2:
                    if (w)
                        gotBit(0);
                    else
                        return -1;
                    break;
                case T3:
                    if (w == 0)
                        gotBit(1);
                    else
                        return -1;
                    break;
            }
        } else if (width >= 2500 && 8 * pos + bits == 12) {
            for (byte i = 0; i < 4; ++i)
                gotBit(0);
            alignTail(2);
            return 1;
        } else
            return -1;
        return 0;
    }

/* ======================================================
 * XrfDecoder
 * ------------------------------------------------------
 */
	XrfDecoder::XrfDecoder () {}

    // see also http://davehouston.net/rf.htm
	int XrfDecoder::decode (word width) {
        if (width > 2000 && pos >= 4)
            return 1;
        if (width > 5000)
            return -1;
        if (width > 4000 && state == UNKNOWN)
            state = OK;
        else if (350 <= width && width < 1800) {
            byte w = width >= 720;
            switch (state) {
                case OK:
                    if (w == 0)
                        state = T0;
                    else
                        return -1;
                    break;
                case T0:
                    gotBit(w);
                    break;
            }
        } else
            return -1;
        return 0;
    }

/* ======================================================
 * HezDecoder
 * ------------------------------------------------------
 */
	HezDecoder::HezDecoder () {}

    // see also http://homeeasyhacking.wikia.com/wiki/Home_Easy_Hacking_Wiki
	int HezDecoder::decode (word width) {
        if (200 <= width && width < 1200) {
            byte w = width >= 600;
            gotBit(w);
        } else if (width >= 5000 && pos >= 5 /*&& 8 * pos + bits == 50*/) {
            for (byte i = 0; i < 6; ++i)
                gotBit(0);
            alignTail(7); // keep last 56 bits
            return 1;
        } else
            return -1;
        return 0;
    }


/* ======================================================
 * VisonicDecoder
 * ------------------------------------------------------
 */
// 868 MHz decoders
	VisonicDecoder::VisonicDecoder () {}

	int VisonicDecoder::decode (word width) {
        if (200 <= width && width < 1000) {
            byte w = width >= 600;
            switch (state) {
                case UNKNOWN:
                case OK:
                    state = w == 0 ? T0 : T1;
                    break;
                case T0:
                    gotBit(!w);
                    if (w)
                        return 0;
                    break;
                case T1:
                    gotBit(!w);
                    if (!w)
                        return 0;
                    break;
            }
            // sync error, flip all the preceding bits to resync
            for (byte i = 0; i <= pos; ++i)
                data[i] ^= 0xFF;
        } else if (width >= 2500 && 8 * pos + bits >= 36 && state == OK) {
            for (byte i = 0; i < 4; ++i)
                gotBit(0);
            alignTail(5); // keep last 40 bits
            // only report valid packets
            byte b = data[0] ^ data[1] ^ data[2] ^ data[3] ^ data[4];
            if ((b & 0xF) == (b >> 4))
                return 1;
        } else
            return -1;
        return 0;
    }

/* ======================================================
 * EMxDecoder
 * ------------------------------------------------------
 */
	EMxDecoder::EMxDecoder () {}

    // see also http://fhz4linux.info/tiki-index.php?page=EM+Protocol
	int EMxDecoder::decode (word width) {
        if (200 <= width && width < 1000) {
            byte w = width >= 600;
            switch (state) {
                case UNKNOWN:
                    if (w == 0)
                        ++flip;
                    else if (flip > 20)
                        state = OK;
                    else
                        return -1;
                    break;
                case OK:
                    if (w == 0)
                        state = T0;
                    else
                        return -1;
                    break;
                case T0:
                    gotBit(w);
                    break;
            }
        } else if (width >= 1500 && pos >= 9)
            return 1;
        else
            return -1;
        return 0;
    }

/* ======================================================
 * KSxDecoder
 * ------------------------------------------------------
 */
	KSxDecoder::KSxDecoder () {}

    // see also http://www.dc3yc.homepage.t-online.de/protocol.htm
	int KSxDecoder::decode (word width) {
        if (200 <= width && width < 1000) {
            byte w = width >= 600;
            switch (state) {
                case UNKNOWN:
                    gotBit(w);
                    bits = pos = 0;
                    if (data[0] != 0x95)
                        state = UNKNOWN;
                    break;
                case OK:
                    state = w == 0 ? T0 : T1;
                    break;
                case T0:
                    gotBit(1);
                    if (!w)
                        return -1;
                    break;
                case T1:
                    gotBit(0);
                    if (w)
                        return -1;
                    break;
            }
        } else if (width >= 1500 && pos >= 6)
            return 1;
        else
            return -1;
        return 0;
    }

/* ======================================================
 * FSxDecoder
 * ------------------------------------------------------
 */
	FSxDecoder::FSxDecoder () {}

    // see also http://fhz4linux.info/tiki-index.php?page=FS20%20Protocol
	int FSxDecoder::decode (word width) {
        if (300 <= width && width < 775) {
            byte w = width >= 500;
            switch (state) {
                case UNKNOWN:
                    if (w == 0)
                        ++flip;
                    else if (flip > 20)
                        state = T1;
                    else
                        return -1;
                    break;
                case OK:
                    state = w == 0 ? T0 : T1;
                    break;
                case T0:
                    gotBit(0);
                    if (w)
                        return -1;
                    break;
                case T1:
                    gotBit(1);
                    if (!w)
                        return -1;
                    break;
            }
        } else if (width >= 1500 && pos >= 5)
            return 1;
        else
            return -1;
        return 0;
    }

/* ======================================================
 * RemoteControlProtocol1
 * ------------------------------------------------------
 */
	RCSwitch_::RCSwitch_()  {}

	void RCSwitch_::configure(int protocol,RCSwitch * _transmit) {
		  this->resetDecoder();
		  this->setRepeatTransmit(10);
		  this->setReceiveTolerance(60);
		  this->setProtocol(protocol);
		  this->changeCount = 0;
		  this->rcs = _transmit;
	}

	int RCSwitch_::decode (word duration) {

		  int _changeCount;

		  // Signal starts with a Sync message, this one is between 6ms and 10ms
		  // it finished with the same sync message.
		  // if duration is > 5000 we can be in this sync period (begin or end)
		  if (   changeCount > 6					/* this looks like end */
			  && duration > 5000
		      && duration > this->timings[0] - 200
		      && duration < this->timings[0] + 200
		     ) {

			  _changeCount = changeCount-1;
			  changeCount = 0;
			  switch (this->nProtocol) {
				  case 1: if (receiveProtocol1(_changeCount) == true) return 1; break;
				  case 2: if (receiveProtocol2(_changeCount) == true) return 1; break;
			  }
			  return -1;

		  } else if (duration > 5000) { /* otherwise it's just the begining */
		      changeCount = 0;
		  }

		  // by default we are in the message itself
		  if (changeCount >= RCSWITCH_MAX_CHANGES) {
		    changeCount = 0;
		  }
		  this->timings[changeCount++] = duration;
		  return 0;
	}

	/**
	  * Sets the protocol to send.
	  */
	void RCSwitch_::setProtocol(int nProtocol) {
	  this->nProtocol = nProtocol;
	  if (nProtocol == 1){
		  this->setPulseLength(350);
	  }
	  else if (nProtocol == 2) {
		  this->setPulseLength(650);
	  }
	}

	/**
	  * Sets the protocol to send with pulse length in microseconds.
	  */
	void RCSwitch_::setProtocol(int nProtocol, int nPulseLength) {
	  this->nProtocol = nProtocol;
	  if (nProtocol == 1){
		  this->setPulseLength(nPulseLength);
	  }
	  else if (nProtocol == 2) {
		  this->setPulseLength(nPulseLength);
	  }
	}


	/**
	  * Sets pulse length in microseconds
	  */
	void RCSwitch_::setPulseLength(int nPulseLength) {
	  this->nPulseLength = nPulseLength;
	}

	/**
	 * Sets Repeat Transmits
	 */
	void RCSwitch_::setRepeatTransmit(int nRepeatTransmit) {
	  this->nRepeatTransmit = nRepeatTransmit;
	}

	/**
	 * Set Receiving Tolerance
	 */
	void RCSwitch_::setReceiveTolerance(int nPercent) {
		this->nReceiveTolerance = nPercent;
	}


	bool RCSwitch_::receiveProtocol1(unsigned int _changeCount){

	      unsigned long delay = this->timings[0] / 31;
	      unsigned long delayTolerance = delay * this->nReceiveTolerance * 0.01;

	      for (unsigned int i = 1; i<_changeCount ; i=i+2) {

	          if (    this->timings[i] > delay-delayTolerance
	        	   && this->timings[i] < delay+delayTolerance
	        	   && this->timings[i+1] > delay*3-delayTolerance
	        	   && this->timings[i+1] < delay*3+delayTolerance) {
	        	   this->gotBit(0);
	          } else if (    this->timings[i] > delay*3-delayTolerance
	        		      && this->timings[i] < delay*3+delayTolerance
	        		      && this->timings[i+1] > delay-delayTolerance
	        		      && this->timings[i+1] < delay+delayTolerance) {
	        	  this->gotBit(1);
	          } else {
	            // Failed
	        	return false;
	          }
	      }
	      if (_changeCount > 6) {    // ignore < 4bit values as there are no devices sending 4bit values => noise
		     this->reverseData();
		     return true;
	      }
	      return false;

	}

	bool RCSwitch_::receiveProtocol2(unsigned int _changeCount){

	      unsigned long delay = this->timings[0] / 10;
	      unsigned long delayTolerance = delay * this->nReceiveTolerance * 0.01;

	      for (unsigned int i = 1; i<_changeCount ; i=i+2) {

	          if (   this->timings[i] > delay-delayTolerance
	        	  && this->timings[i] < delay+delayTolerance
	        	  && this->timings[i+1] > delay*2-delayTolerance
	        	  && this->timings[i+1] < delay*2+delayTolerance) {
          	    this->gotBit(0);
	          } else if (    this->timings[i] > delay*2-delayTolerance
	        		      && this->timings[i] < delay*2+delayTolerance
	        		      && this->timings[i+1] > delay-delayTolerance
	        		      && this->timings[i+1] < delay+delayTolerance) {
        	    this->gotBit(1);
	          } else {
	            // Failed
	        	return false;
	          }
	      }
	      if (_changeCount > 6) {    // ignore < 4bit values as there are no devices sending 4bit values => noise
   	        this->reverseData();
	    	return true;
	      }
	      return false;
	}

	// =====================================================================
	// Transmit
	// =====================================================================

	void RCSwitch_::send(unsigned long Code, unsigned int length) {
	  this->send( this->dec2binWzerofill(Code, length) );
	}

	void RCSwitch_::send(char* sCodeWord) {
	  for (int nRepeat=0; nRepeat<nRepeatTransmit; nRepeat++) {
	    int i = 0;
	    while (sCodeWord[i] != '\0') {
	      switch(sCodeWord[i]) {
	        case '0':
	          this->send0();
	        break;
	        case '1':
	          this->send1();
	        break;
	      }
	      i++;
	    }
	    this->sendSync();
	  }
	}

	/**
	 * Sends a "0" Bit
	 *                       _
	 * Waveform Protocol 1: | |___
	 *                       _
	 * Waveform Protocol 2: | |__
	 */
	void RCSwitch_::send0() {
		if (this->nProtocol == 1){
			this->rcs->transmit(1*this->nPulseLength,3*this->nPulseLength);
		}
		else if (this->nProtocol == 2) {
			this->rcs->transmit(1*this->nPulseLength,2*this->nPulseLength);
		}
	}

	/**
	 * Sends a "1" Bit
	 *                       ___
	 * Waveform Protocol 1: |   |_
	 *                       __
	 * Waveform Protocol 2: |  |_
	 */
	void RCSwitch_::send1() {
	  	if (this->nProtocol == 1){
			this->rcs->transmit(3*this->nPulseLength,1*this->nPulseLength);
		}
		else if (this->nProtocol == 2) {
			this->rcs->transmit(2*this->nPulseLength,1*this->nPulseLength);
		}
	}

	/**
	 * Sends a Tri-State "0" Bit
	 *            _     _
	 * Waveform: | |___| |___
	 */
	void RCSwitch_::sendT0() {
	  this->rcs->transmit(1*this->nPulseLength,3*this->nPulseLength);
	  this->rcs->transmit(1*this->nPulseLength,3*this->nPulseLength);
	}

	/**
	 * Sends a Tri-State "1" Bit
	 *            ___   ___
	 * Waveform: |   |_|   |_
	 */
	void RCSwitch_::sendT1() {
	  this->rcs->transmit(3*this->nPulseLength,1*this->nPulseLength);
	  this->rcs->transmit(3*this->nPulseLength,1*this->nPulseLength);
	}

	/**
	 * Sends a Tri-State "F" Bit
	 *            _     ___
	 * Waveform: | |___|   |_
	 */
	void RCSwitch_::sendTF() {
	  this->rcs->transmit(1*this->nPulseLength,3*this->nPulseLength);
	  this->rcs->transmit(3*this->nPulseLength,1*this->nPulseLength);
	}

	/**
	 * Sends a "Sync" Bit
	 *                       _
	 * Waveform Protocol 1: | |_______________________________
	 *                       _
	 * Waveform Protocol 2: | |__________
	 */
	void RCSwitch_::sendSync() {

	    if (this->nProtocol == 1){
			this->rcs->transmit(1*this->nPulseLength,31*this->nPulseLength);
		}
		else if (this->nProtocol == 2) {
			this->rcs->transmit(1*this->nPulseLength,10*this->nPulseLength);
		}
	}

	char * RCSwitch_::dec2binWzerofill(unsigned long Dec, unsigned int bitLength){
	  static char bin[64];
	  unsigned int i=0;

	  while (Dec > 0) {
	    bin[32+i++] = ((Dec & 1) > 0) ? '1' : '0';
	    Dec = Dec >> 1;
	  }

	  for (unsigned int j = 0; j< bitLength; j++) {
	    if (j >= bitLength - i) {
	      bin[j] = bin[ 31 + i - (j - (bitLength - i)) ];
	    }else {
	      bin[j] = '0';
	    }
	  }
	  bin[bitLength] = '\0';

	  return bin;
	}
