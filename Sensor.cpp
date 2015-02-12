/* ===================================================
Sensor.cpp
* ====================================================
* Sensor decoding from 433 Message
*
* Created on: 17 sept. 2013
* Author: disk91 / (c) http://www.disk91.com
*
* added comme sensor Oregon
* to decode

* ===================================================
*/
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include "Sensor.h"

//#define SENSORDEBUG // Large debug trace
//#define SENSORTRACE // Small debug trace to verify error only

char Sensor::_hexDecod[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

// ——————————————————
// Construction – init variable then call decode function
Sensor::Sensor( char * _strval ) {

this->temperature = 0.0;
this->humidity = 0.0;
this->channel = -1;
this->direction = -1;
this->speed = 0;
this->battery = false;
this->haveTemperature = false;
this->haveHumidity = false;
this->haveBattery = false;
this->isValid = false;
this->haveChannel = false;
this->haveDirection = false;
this->haveRain = false;
this->rain = 0;
this->haveTrain = false;
this->train = 0;
this->haveSpeed = false;
this->sensorName[0]='\0';
this->sensorType = -1;
this->havePressure = false;
this->pressure = 0;

this->sensorClass = SENS_CLASS_NONE;
// time(&this->creationTime);

}

// —————————————————
// availablePressure() – return true if valid && have wind speed
bool Sensor::availablePressure(){
return (this->isValid && this->havePressure);
}
// —————————————————
// availableSpeed() – return true if valid && have wind speed
bool Sensor::availableSpeed(){
return (this->isValid && this->haveSpeed);
}

// —————————————————
// availableDirection() – return true if valid && have wind direction
bool Sensor::availableDirection(){
return (this->isValid && this->haveDirection);
}
// —————————————————
// availableRain() – return true if valid && have Rain
bool Sensor::availableRain(){
return (this->isValid && this->haveRain);
}

// —————————————————
// availableTemp() – return true if valid && have Temp
bool Sensor::availableTemp(){
return (this->isValid && this->haveTemperature);
}
// —————————————————
// availableHumidity() – return true if valid && have Humidity
bool Sensor::availableHumidity() {
return (this->isValid && this->haveHumidity);
}
// —————————————————
// isBatteryLow() – return true if valid && haveBattery && flag set
bool Sensor::isBatteryLow(){
return (this->isValid && this->haveBattery && this->battery);
}
// —————————————————
// getPressure() – return Wind speed in Mb
double Sensor::getPressure(){
return this->pressure;
}

// —————————————————
// getSpeed() – return Wind speed in Degre°
double Sensor::getSpeed(){
return this->speed;
}
// —————————————————
// getDirection() – return Wind direction in Degre°
double Sensor::getDirection(){
return this->direction;
}
// —————————————————
// getRain() – return Rain in mm/h
double Sensor::getRain(){
return this->rain;
}
// —————————————————
// getTemperature() – return temperature in C°
double Sensor::getTemperature(){
return this->temperature;
}
// —————————————————
// getHumidity() – return humidity in % (base 100)
double Sensor::getHumidity() {
return this->humidity;
}

// —————————————————
// haveChannel() – return true if valid && haveChannel
bool Sensor::hasChannel() {
return (this->isValid && this->haveChannel);
}
// —————————————————
// isDecoded() – return true if valid
bool Sensor::isDecoded() {
return (this->isValid);
}

// —————————————————
// getChannel() – return channel (1,2,3)
int Sensor::getChannel() {
return this->channel;
}

// —————————————————
// getCreationTime() – return object creation time
//time_t Sensor::getCreationTime() {
// return this->creationTime;
// }

// —————————————————
// getSensClass() – return sensor class
int Sensor::getSensClass() {
return this->sensorClass;
}

// —————————————————
// getSensType() – return sensor type
int Sensor::getSensType() {
return this->sensorType;
}

// —————————————————
// getSensorName() – return sensor name
char * Sensor::getSensorName() {
return this->sensorName;
}

/* Eliminado por error
// —————————————————
// getIntFromChar() – (-1 if error)
int Sensor::getIntFromChar(char c) {
for (int i=0;i = 10) return -1;
r+=t;
s++;
}
return (double)(r/10.0);
}
*/


/*Copiado de original*/

// ---------------------------------------------------
// getIntFromChar() - (-1 if error)
int Sensor::getIntFromChar(char c) {
	for ( int i=0 ; i < 16 ; i++ )
	  if ( _hexDecod[i] == c ) return i;
	return -1;
}

// ---------------------------------------------------
// getIntFromString() - get an unsigned int value from
//   the given string. -1 if error
int Sensor::getIntFromString(char * s) {
	int r = 0;
	while ( *s != '\0' ) {
	    r *= 16;
		int t = getIntFromChar(*s);
		if ( t == -1 ) return -1;
		r += t;
		s++;
	}
	return r;
}

// --------------------------------------------------
// getDoubleFromString() - get double value related to
//                         BCD encoded string XXX = XX.X
double Sensor::getDoubleFromString(char * s) {
	int r = 0;
	while ( *s != '\0' ) {
	    r *= 10;
		int t = getIntFromChar(*s);
		if ( t == -1 || t >= 10 ) return -1;
		r += t;
		s++;
	}
	return (double)( r / 10.0 );
}

/*Fin copiado de original*/

// ==================================================================

// ——————————————————
// Build the right sensor type by identifying the
// header code
Sensor * Sensor::getRightSensor(char * s) {
int len = strlen(s);
if (len > 4) {
#ifdef SENSORDEBUG
printf("Sensor::getRightSensor – create of (%s)\n",s);
#endif

if (s[0] == 'O' && s[1] == 'S' && s[2] == 'V' && s[3] == '2') {
#ifdef SENSORDEBUG
printf("Sensor::getRightSensor – create OregonSensorV2\n");
#endif
OregonSensorV2 * r = new OregonSensorV2(s);
return (Sensor *) r;
}
}
return NULL;
}

// ==================================================================

OregonSensorV2::OregonSensorV2(char * _strval) : Sensor(_strval) {
this->sensorClass = SENS_CLASS_OS;
this->isValid = this->decode(_strval);
}

// —————————————————————————————
// Decode OregonScientific V2 protocol for specific
// Oregon Devices
//
// Support :
// – THGR122NX : Temp + Humidity
// OSV2 1A2D1002 502060552A4C
// In correct order :
// OSV2 A 1D20 1 20 0 502 0 655 2A 4C
// OSV2 => decoding header
// A => Sync header
// 1D20 => THGR122NX ID
// – THN132N : Temp
// OSV2 EA4C20809822D013
// In correct order :
// OSV2 A EC40 2 08 8 922 D0 13
//
// ——————————————————————————————
bool OregonSensorV2::decode(char * _str) {
char * pt = & _str[5];
int len = strlen(_str);
char sensorId[5]; int isensorId;

// Proceed the right sensor
if (len > 11) {
sensorId[0] = pt[0];sensorId[1] = pt[3];sensorId[2] = pt[2];sensorId[3] = pt[5];sensorId[4] ='\0';
isensorId = getIntFromString(sensorId);
#ifdef SENSORDEBUG
printf("OSV2 – decode : id(%s)(0x%4X)\n",sensorId, isensorId);
#endif

switch (isensorId) {

case 0x5D60:
this->sensorType=0x5D60;
this->sensorName[0] ='B';
this->sensorName[1] ='T';
this->sensorName[2] ='H';
this->sensorName[3] ='G';
this->sensorName[4] ='9';
this->sensorName[5] ='6';
this->sensorName[6] ='8';
this->sensorName[7] ='\0';
return decode_BTHG968(pt); break;

case 0x1D20:
this->sensorType=0x1D20;
this->sensorName[0] ='T';
this->sensorName[1] ='H';
this->sensorName[2] ='G';
this->sensorName[3] ='R';
this->sensorName[4] ='1';
this->sensorName[5] ='2';
this->sensorName[6] ='2';
this->sensorName[7] ='N';
this->sensorName[8] ='X';
this->sensorName[9] ='\0';
return decode_THGR122NX(pt); break;

case 0x1D30:
this->sensorType=0x1D30;
this->sensorName[0] ='T';
this->sensorName[1] ='H';
this->sensorName[2] ='G';
this->sensorName[3] ='R';
this->sensorName[4] ='N';
this->sensorName[5] ='2';
this->sensorName[6] ='2';
this->sensorName[7] ='8';
this->sensorName[8] ='N';
this->sensorName[9] ='X';
this->sensorName[10] ='\0';
return decode_THGRN228NX(pt); break;

case 0xEC40:
this->sensorType=0xEC40;
this->sensorName[0] ='T';
this->sensorName[1] ='H';
this->sensorName[2] ='N';
this->sensorName[3] ='1';
this->sensorName[4] ='3';
this->sensorName[5] ='2';
this->sensorName[6] ='N';
this->sensorName[7] ='\0';
return decode_THN132N(pt); break;

case 0x3D00:
this->sensorType=0x3D00;
this->sensorName[0] ='W';
this->sensorName[1] ='G';
this->sensorName[2] ='R';
this->sensorName[3] ='9';
this->sensorName[4] ='1';
this->sensorName[5] ='8';
this->sensorName[6] ='\0';
return decode_WGR918(pt); break;

case 0x2D10:
this->sensorType=0x2D10;
this->sensorName[0] ='R';
this->sensorName[1] ='G';
this->sensorName[2] ='R';
this->sensorName[3] ='9';
this->sensorName[4] ='1';
this->sensorName[5] ='8';
this->sensorName[6] ='\0';
return decode_RGR918(pt); break;

default:
return false;

}

}
return false;
}

// —————————————————————————————
// Decode OregonScientific V2 protocol for specific
// Oregon Devices
// – BTHG968 temperature + hygro + pression atmospherique
// ——————————————————————————————
bool OregonSensorV2::decode_BTHG968(char * pt) {

char temp[4]; double dtemp; // Temp in BCD
char tempS; int itempS; // Sign 0 = positif
char humid[4]; double dhumid; // Humid in BCD
char pressure[3]; double ipressure;

char crc[3]; int icrc;
int len = strlen(pt);

if ( len == 20 ) {
temp[0] = pt[10] ; temp[1] = pt[11] ; temp[2] = pt[8] ; temp[3] ='\0';
tempS = pt[13];
humid[0] = pt[15] ; humid[1] = pt[12]; humid[2] = '0' ; humid[3] ='\0';
pressure[0] = pt[16];pressure[1]=pt[17];pressure[2]='\0';
crc[0] = pt[18] ; crc[1] = pt[19] ; crc[2] ='\0';

#ifdef SENSORDEBUG
printf("OSV2 – decode : id(%s) temp(%s) sign(%c) humid(%s) pressure(%s) crc(%s)\n", "5D60",temp,tempS,humid, pressure, crc);
#endif

// Conversion to int value
itempS = getIntFromChar(tempS) & 0x08;
icrc = getIntFromString(crc);
dtemp = getDoubleFromString(temp);
dhumid = getDoubleFromString(humid);
ipressure = getIntFromString(pressure);

#ifdef SENSORDEBUG
printf("OSV2 – decode : id(0x%04X) temp(%f) sign(%d) humid(%d) pressure(%d) cksum(0x%02X) crc(0x%02X)\n", 0x5D60,dtemp,itempS,dhumid, ipressure, icrc);
#endif

// Check SUM & CRC
// if ( validate(pt,16,icrc,ichecksum) == true ) {

// now we can decode the important flag and fill the object
this->haveTemperature = true;
this->temperature = (itempS == 0)?dtemp:-dtemp;
this->haveHumidity = true;
this->humidity = dhumid;
this->havePressure = true;
this->pressure = (856 + ipressure);

return true;
// } else return false;

}
return false;
}

// —————————————————————————————
// Decode OregonScientific V2 protocol for specific
// Oregon Devices
// – RGR918 PLUVIOMETRE
// ——————————————————————————————

bool OregonSensorV2::decode_RGR918(char * pt) {

char rain[4]; double irain; // Temp
char checksum[19]; int ichecksum;
char crc[3]; int icrc;
int len = strlen(pt);

if ( len == 20 ) {
//channel = pt[4];
rain[0] = pt[10] ; rain[1] = pt[11] ; rain[2] = pt[8] ; rain[3] ='\0';
checksum[0] = pt[18]; checksum[1] = pt[19]; checksum[2] ='\0';
crc[0] = pt[18] ; crc[1] = pt[19] ; crc[2] ='\0';

#ifdef SENSORDEBUG

printf("OSV2 – decode raw: id(%s) rain(%s) cksum(%s) crc(%s)\n", "2D10",rain,checksum,crc);
#endif
// Conversion to int value
ichecksum = getIntFromString(checksum);
irain = getDoubleFromString(rain);
icrc = getIntFromString(crc);
#ifdef SENSORDEBUG
printf("OSV2 – decode : id(0x%04X) rain(%f) cksum(0x%02X) crc(0x%02X) \n", 0x2D10,irain,ichecksum,icrc);
#endif

// Check SUM & CRC
// if ( validate(pt,16,icrc,ichecksum) == true ) {

// now we can decode the important flag and fill the object
this->haveRain = true;
this->rain = (10 * irain);
return true;

// } else return false;

}
return false;
}

// —————————————————————————————
// Decode OregonScientific V2 protocol for specific
// Oregon Devices
// – WGR918 ANEMOMETER
// ——————————————————————————————
bool OregonSensorV2::decode_WGR918(char * pt) {

char dir[4]; double idir; // direction en degres
char speed[4]; double ispeed; // vitesse en km/h
char checksum[3]; int ichecksum;
char crc[3]; int icrc;

int len = strlen(pt);

if ( len == 20 ) {
dir[0] = pt[10] ; dir[1] = pt[11] ; dir[2] = pt[8] ; dir[3] ='\0';
speed[0] = pt[12] ; speed[1] = pt[13]; speed[2] = pt[14]; speed[3] ='\0';
checksum[0] = pt[16]; checksum[1] = pt[17]; checksum[2] ='\0';
crc[0] = pt[18] ; crc[1] = pt[19] ; crc[2] ='\0';
#ifdef SENSORDEBUG
printf("OSV2 – decode : id(%s) dir(%s) speed(%s) cksum(%s) crc(%s)\n", "3D00",dir,speed, checksum, crc);
#endif

// Conversion to int value
ichecksum = getIntFromString(checksum);
icrc = getIntFromString(crc);
idir = getDoubleFromString(dir);
ispeed = getDoubleFromString(speed);

#ifdef SENSORDEBUG
printf("OSV2 – decode : id(0x%04X) dir(%f) speed(%f) cksum(0x%02X) crc(0x%02X)\n",0x3D00,idir,ispeed, ichecksum,icrc);
#endif

// Check SUM & CRC
// if ( validate(pt,16,icrc,ichecksum) == true ) {

// now we can decode the important flag and fill the object
this->haveDirection = true;
this->direction = (10 * idir);
this->haveSpeed = true;
this->speed = (0.1 * ispeed)* 3.6;
return true;
// } else return false;

}
return false;
}

// —————————————————————————————
// Decode OregonScientific V2 protocol for specific
// Oregon Devices
// – THGRN228NX : Temp + Humidity
// ——————————————————————————————
bool OregonSensorV2::decode_THGRN228NX(char * pt) {

char channel; int ichannel; // values 1,2,4
char rolling[3]; int irolling;
char battery; int ibattery; // value & 0x4
char temp[4]; double dtemp; // Temp in BCD
char tempS; int itempS; // Sign 0 = positif
char humid[4]; double dhumid; // Humid in BCD
char checksum[3]; int ichecksum;
char crc[3]; int icrc;
int len = strlen(pt);

if ( len == 20 ) {
channel = pt[4];
rolling[0]=pt[7]; rolling[1]=pt[6]; rolling[2]='\0';
battery = pt[9];
temp[0] = pt[10] ; temp[1] = pt[11] ; temp[2] = pt[8] ; temp[3] ='\0';
tempS = pt[13];
humid[0] = pt[15] ; humid[1] = pt[12]; humid[2] = '0' ; humid[3] ='\0';
checksum[0] = pt[16]; checksum[1] = pt[17]; checksum[2] ='\0';
crc[0] = pt[18] ; crc[1] = pt[19] ; crc[2] ='\0';

#ifdef SENSORDEBUG
printf("OSV2 – decode : id(%s) ch(%c) bat(%c) temp(%s) sign(%c) humid(%s) cksum(%s) crc(%s)\n","1D30",channel,battery,temp,tempS,humid, checksum, crc);
#endif

// Conversion to int value
ichannel = getIntFromChar(channel);
irolling = getIntFromString(rolling);
ibattery = getIntFromChar(battery);
itempS = getIntFromChar(tempS) & 0x08;
ichecksum = getIntFromString(checksum);
icrc = getIntFromString(crc);
dtemp = getDoubleFromString(temp);
dhumid = getDoubleFromString(humid);

#ifdef SENSORDEBUG
printf("OSV2 – decode : id(0x%04X) ch(%d) bat(%d) temp(%f) sign(%d) humid(%f) cksum(0x%02X) crc(0x%02X)\n",0x1D30,ichannel,ibattery,dtemp,itempS,dhumid, ichecksum, icrc);
#endif

// Check SUM & CRC
if ( validate(pt,16,icrc,ichecksum) == true ) {

// now we can decode the important flag and fill the object
this->haveChannel = true;
this->channel = (ichannel != 4)?ichannel:3;
this->haveBattery = true;
this->battery = (ibattery & 0x4);
this->haveTemperature = true;
this->temperature = (itempS == 0)?dtemp:-dtemp;
this->haveHumidity = true;
this->humidity = dhumid;
return true;
} else return false;

}
return false;
}

// —————————————————————————————
// Decode OregonScientific V2 protocol for specific
// Oregon Devices
// – THGR122NX : Temp + Humidity
// ——————————————————————————————
bool OregonSensorV2::decode_THGR122NX(char * pt) {

char channel; int ichannel; // values 1,2,4
char rolling[3]; int irolling;
char battery; int ibattery; // value & 0x4
char temp[4]; double dtemp; // Temp in BCD
char tempS; int itempS; // Sign 0 = positif
char humid[4]; double dhumid; // Humid in BCD
char checksum[3]; int ichecksum;
char crc[3]; int icrc;
int len = strlen(pt);

if ( len == 20 ) {
channel = pt[4];
rolling[0]=pt[7]; rolling[1]=pt[6]; rolling[2]='\0';
battery = pt[9];
temp[0] = pt[10] ; temp[1] = pt[11] ; temp[2] = pt[8] ; temp[3] ='\0';
tempS = pt[13];
humid[0] = pt[15] ; humid[1] = pt[12]; humid[2] = '0' ; humid[3] ='\0';
checksum[0] = pt[16]; checksum[1] = pt[17]; checksum[2] ='\0';
crc[0] = pt[18] ; crc[1] = pt[19] ; crc[2] ='\0';

#ifdef SENSORDEBUG
printf("OSV2 – decode : id(%s) ch(%c) bat(%c) temp(%s) sign(%c) humid(%s) cksum(%s) crc(%s)\n","1D20",channel,battery,temp,tempS,humid, checksum, crc);
#endif

// Conversion to int value
ichannel = getIntFromChar(channel);
irolling = getIntFromString(rolling);
ibattery = getIntFromChar(battery);
itempS = getIntFromChar(tempS) & 0x08;
ichecksum = getIntFromString(checksum);
icrc = getIntFromString(crc);
dtemp = getDoubleFromString(temp);
dhumid = getDoubleFromString(humid);

#ifdef SENSORDEBUG
printf("OSV2 – decode : id(0x%04X) ch(%d) bat(%d) temp(%f) sign(%d) humid(%f) cksum(0x%02X) crc(0x%02X)\n",0x1D20,ichannel,ibattery,dtemp,itempS,dhumid, ichecksum, icrc);
#endif

// Check SUM & CRC
if ( validate(pt,16,icrc,ichecksum) == true ) {

// now we can decode the important flag and fill the object
this->haveChannel = true;
this->channel = (ichannel != 4)?ichannel:3;
this->haveBattery = true;
this->battery = (ibattery & 0x4);
this->haveTemperature = true;
this->temperature = (itempS == 0)?dtemp:-dtemp;
this->haveHumidity = true;
this->humidity = dhumid;
return true;
} else return false;

}
return false;
}

// —————————————————————————————
// Decode OregonScientific V2 protocol for specific
// Oregon Devices
// – THN132N : Temp
// ——————————————————————————————
bool OregonSensorV2::decode_THN132N(char * pt) {

char channel; int ichannel; // values 1,2,4
char rolling[3]; int irolling;
char battery; int ibattery; // value & 0x4
char temp[4]; double dtemp; // Temp in BCD
char tempS; int itempS; // Sign 0 = positif
char checksum[3]; int ichecksum;
int len = strlen(pt);

if ( len == 20 ) {

channel = pt[4];
rolling[0]=pt[7]; rolling[1]=pt[6]; rolling[2]='\0';
battery = pt[9];
temp[0] = pt[10] ; temp[1] = pt[11] ; temp[2] = pt[8] ; temp[3] ='\0';
tempS = pt[13];
checksum[0] = pt[15]; checksum[1] = pt[12]; checksum[2] ='\0';

#ifdef SENSORDEBUG
printf("OSV2 – decode : id(%s) ch(%c) bat(%c) temp(%s) sign(%c) cksum(%s) \n","EC040",channel,battery,temp,tempS, checksum);
#endif

// Conversion to int value
ichannel = getIntFromChar(channel);
irolling = getIntFromString(rolling);
ibattery = getIntFromChar(battery) & 0x04;
itempS = getIntFromChar(tempS) & 0x08;
ichecksum = getIntFromString(checksum);
dtemp = getDoubleFromString(temp);

// Check SUM
int _sum = getIntFromChar(pt[0]);
for ( int i = 2 ; i <= 11 ; i++ ) _sum += getIntFromChar(pt[i]);
_sum += getIntFromChar(pt[13]);


#ifdef SENSORDEBUG
printf("OSV2 - decode : id(0x%04X) ch(%d) bat(%d) temp(%f) sign(%d) cksum(0x%02X) _chksum(0x%02X)\n",0xEC40,ichannel,ibattery,dtemp,itempS, ichecksum, _sum);
#endif

if ( _sum == ichecksum ){
haveChannel = true;
this->channel = (ichannel != 4)?ichannel:3;
this->haveBattery = true;
this->battery = (ibattery != 0);
this->haveTemperature = true;
this->temperature = (itempS == 0)?dtemp:-dtemp;
this->haveHumidity = false;
return true;
} else return false;

}
return false;
}

// —————————————————–
// Validate CRC and Checksum value from the signal
// Starts at the Sync header digit
// return true if both are valid
bool OregonSensorV2::validate(char * _str, int _len, int _CRC, int _SUM) {

int i,j,c,CRC,SUM;
CRC =0x43;
int CCIT_POLY = 0x07;
SUM = 0x00;

// swap each 2 digit
char __str[100];
for (j=0 ; j < _len ; j+=2){
__str[j] = _str[j+1];
__str[j+1] = _str[j];
}
__str[_len]='\0'; // recopie de

for (j=1; j< _len; j++)
{
c = getIntFromChar(__str[j]);
SUM += c;
CRC ^= c;

// Because we have to skip the rolling value in the CRC computation
if ( j != 6 && j != 7 ) {
for(i = 0; i < 4; i++) {
if( (CRC & 0x80) != 0 )
CRC = ( (CRC << 1) ^ CCIT_POLY ) & 0xFF;
else
CRC = (CRC << 1 ) & 0xFF;
}
}
}

// CRC is 8b but the len is quartet based and we start are digit 1
if ( ! (_len & 1) ) {
for(i = 0; i<4; i++) {
if( (CRC & 0x80) != 0 )
CRC = ( (CRC << 1) ^ CCIT_POLY ) & 0xFF;
else
CRC = (CRC << 1 ) & 0xFF;
}
}

#ifdef SENSORDEBUG
printf("Validate OOK – SUM : 0x%02X(0x%02X) CRC : 0x%02X(0x%02X)\n",SUM,_SUM,CRC,_CRC);
#endif
// Do not check crc anymore as depend on sensor it is not working as expected
if ( SUM == _SUM /* && CRC == _CRC */ ) return true;
else {

#ifdef SENSORTRACE
printf("OSV2 – validate err (%s) SUM : 0x%02X(0x%02X) CRC : 0x%02X(0x%02X)\n",_str,SUM,_SUM,CRC,_CRC);
#endif
}
return false;
}
