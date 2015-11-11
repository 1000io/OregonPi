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
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <time.h>
#include <sstream>
#include <iostream>
#include "Sensor.h"

//#define SENSORDEBUG // Large debug trace
//#define SENSORTRACE // Small debug trace to verify error only

const char OregonSensorV2::_sensorId[] = "OSV2";
const char OregonSensorV3::_sensorId[] = "OSV3";
// ——————————————————
// Construction – init variable then call decode function
Sensor::Sensor( ) :
  _temperature(0),
  _humidity(0),
  _rain(0),
  _train(0),
  _direction(-1),
  _speed(0),
  _pressure(0),
  _channel(-1),
  _sensorClass(SENS_CLASS_NONE),
  _sensorType(-1),
  _availableInfos(0)
// time(&creationTime);
{
}

// —————————————————
// availablePressure() – return true if valid && have wind speed
bool Sensor::availablePressure() const {
return (_availableInfos.isFlagsSet(isValid|havePressure));
}
// —————————————————
// availableSpeed() – return true if valid && have wind speed
bool Sensor::availableSpeed() const{
  return (_availableInfos.isFlagsSet(isValid|haveSpeed));
}

// —————————————————
// availableDirection() – return true if valid && have wind direction
bool Sensor::availableDirection() const{
  return (_availableInfos.isFlagsSet(isValid|haveDirection));
}
// —————————————————
// availableRain() – return true if valid && have Rain
bool Sensor::availableRain() const{
  return (_availableInfos.isFlagsSet(isValid|haveRain));
}

// —————————————————
// availableTemp() – return true if valid && have Temp
bool Sensor::availableTemp() const{
  return (_availableInfos.isFlagsSet(isValid|haveTemperature));
}
// —————————————————
// availableHumidity() – return true if valid && have Humidity
bool Sensor::availableHumidity()  const{
  return (_availableInfos.isFlagsSet(isValid|haveHumidity));
}
// —————————————————
// isBatteryLow() – return true if valid && haveBattery && flag set
bool Sensor::isBatteryLow() const{
  return (_availableInfos.isFlagsSet(isValid|haveBattery|battery));
}
// —————————————————
// getPressure() – return Wind speed in Mb
double Sensor::getPressure() const{
return _pressure;
}

// —————————————————
// getSpeed() – return Wind speed in Degre°
double Sensor::getSpeed() const{
return _speed;
}
// —————————————————
// getDirection() – return Wind direction in Degre°
double Sensor::getDirection() const{
return _direction;
}
// —————————————————
// getRain() – return Rain in mm/h
double Sensor::getRain() const{
return _rain;
}
// —————————————————
// getTemperature() – return temperature in C°
double Sensor::getTemperature() const{
return _temperature;
}
// —————————————————
// getHumidity() – return humidity in % (base 100)
double Sensor::getHumidity() const{
return _humidity;
}

// —————————————————
// haveChannel() – return true if valid && haveChannel
bool Sensor::hasChannel() const{
  return (_availableInfos.isFlagsSet(isValid|haveChannel));
}
// —————————————————
// isDecoded() – return true if valid
bool Sensor::isDecoded() const{
return (_availableInfos.isFlagsSet(isValid));
}

// —————————————————
// getChannel() – return channel (1,2,3)
int Sensor::getChannel() const {
return _channel;
}

// —————————————————
// getCreationTime() – return object creation time
//time_t Sensor::getCreationTime() {
// return creationTime;
// }

// —————————————————
// getSensClass() – return sensor class
int Sensor::getSensClass() const {
return _sensorClass;
}

// —————————————————
// getSensType() – return sensor type
int Sensor::getSensType() const {
return _sensorType;
}

// —————————————————
// getSensorName() – return sensor name
const std::string& Sensor::getSensorName() const {
return _sensorName;
}

// ---------------------------------------------------
// getIntFromChar() - get int from hex char(-1 if error)
int Sensor::getIntFromChar(char c) const {
  std::stringstream ss;
  ss << std::hex << c;
  
  int i;
  ss >> i;
  
  if(!ss.fail())
    return i;
  
  return -1;
}

// ---------------------------------------------------
// getIntFromString() - get an unsigned int value from
//   the given string. -1 if error
int Sensor::getIntFromString(char * s) const {
  std::stringstream ss;
  ss << std::hex << s;

  int i;
  ss >> i;

  if(!ss.fail())
    return i;
  
  return -1;
}

// --------------------------------------------------
// getDoubleFromString() - get double value related to
//                         BCD encoded string XXX = XX.X
double Sensor::getDoubleFromString(char * s) const {
  std::stringstream ss;
  ss << std::dec << s;

  int r = -1;
  
  ss >> r;
  
  if(!ss.fail())
    return (r / 10.0);

  return -1.;

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
 
 if(strncmp(s, OregonSensorV2::_sensorId, strlen(OregonSensorV2::_sensorId)) == 0) {
#ifdef SENSORDEBUG
printf("Sensor::getRightSensor – create OregonSensorV2\n");
#endif
 return new OregonSensorV2(s);
 } else if ( strncmp(s, OregonSensorV3::_sensorId, strlen(OregonSensorV3::_sensorId)) == 0) {
#ifdef SENSORDEBUG
printf("Sensor::getRightSensor – create OregonSensorV3\n");
#endif
 return new OregonSensorV3(s);
 }
#ifdef SENSORDEBUG
 else {
   std::cout << "[Sensor::getRightSensor] right length but unknown signature:" << s << std::endl;
 }
#endif 
 } else {
  std::cout << "[Sensor::getRightSensor] dont know how to decode: " << s << std::endl;
 }
 
return NULL;
}

// ==================================================================

OregonSensorV2::OregonSensorV2(char * _strval) : Sensor( ) {
  _sensorClass = SENS_CLASS_OS;
  _availableInfos.setFlags(decode(_strval));
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
  _sensorType=0x5D60;
  _sensorName = "BTHG968";
return decode_BTHG968(pt); break;

case 0x1D20:
_sensorType=0x1D20;
 _sensorName = "THGR122NX"; 
return decode_THGR122NX(pt); break;

case 0x1D30:
  _sensorType=0x1D30;
  _sensorName="THGRN228NX";
    return decode_THGRN228NX(pt); break;

case 0xEC40:
_sensorType=0xEC40;
 _sensorName = "THN132N";
 return decode_THN132N(pt); break;

case 0x3D00:
_sensorType=0x3D00;
 _sensorName = "WGR918";
return decode_WGR918(pt); break;

case 0x2D10:
_sensorType=0x2D10;
 _sensorName = "RGR918";
return decode_RGR918(pt); break;
 
default:
  std::cout << "Unknown sensor id: " << std::hex << isensorId << std::endl;
  return false;
  break;
}
 
 }
 else {
  std::cout << "OSV2 - decode: bad length" << std::endl; 
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

char crc[3];
int icrc;
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
 _availableInfos.setFlags(haveTemperature);
_temperature = (itempS == 0)?dtemp:-dtemp;
 _availableInfos.setFlags(haveHumidity);
_humidity = dhumid;
 _availableInfos.setFlags(havePressure);
_pressure = (856 + ipressure);

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
 _availableInfos.setFlags(haveRain);
_rain = (10 * irain);
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
 _availableInfos.setFlags(haveDirection);
_direction = (10 * idir);
 _availableInfos.setFlags(haveSpeed);
_speed = (0.1 * ispeed)* 3.6;
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
  _availableInfos.setFlags(haveChannel);
_channel = (ichannel != 4)?ichannel:3;
 _availableInfos.setFlags(haveBattery);
 if(ibattery & 0x4) {
   _availableInfos.setFlags(battery);
 }
 _availableInfos.setFlags(haveTemperature);
_temperature = (itempS == 0)?dtemp:-dtemp;
 _availableInfos.setFlags(haveHumidity);
_humidity = dhumid;
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
  _availableInfos.setFlags(haveChannel);
_channel = (ichannel != 4)?ichannel:3;
 _availableInfos.setFlags(haveBattery);
 if(ibattery & 0x4) {
   _availableInfos.setFlags(battery);
 }
 _availableInfos.setFlags(haveTemperature);
_temperature = (itempS == 0)?dtemp:-dtemp;
 _availableInfos.setFlags(haveHumidity);
_humidity = dhumid;
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

if ( len == 16 ) {

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
  _availableInfos.setFlags(haveChannel);
_channel = (ichannel != 4)?ichannel:3;
 _availableInfos.setFlags(haveBattery);
 if(ibattery != 0) {
   _availableInfos.setFlags(battery);
 }
 _availableInfos.setFlags(haveTemperature);
 _temperature = (itempS == 0)?dtemp:-dtemp;
 _availableInfos.setFlags(haveHumidity);
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

OregonSensorV3::OregonSensorV3(char * _strval) : Sensor( ) {
  _sensorClass = SENS_CLASS_OS;
  _availableInfos.setFlags(decode(_strval));
}

bool OregonSensorV3::decode(char * _str) {
  char * pt = & _str[5];
  int len = strlen(_str);
  char sensorId[5]; int isensorId;
  
  // Proceed the right sensor
  if (len > 11) {
    sensorId[0] = pt[0];sensorId[1] = pt[3];sensorId[2] = pt[2];sensorId[3] = pt[5];sensorId[4] ='\0';
    isensorId = getIntFromString(sensorId);
#ifdef SENSORDEBUG
    printf("OSV3 – decode : id(%s)(0x%4X)\n",sensorId, isensorId);
#endif
    
    switch (isensorId) {
    case 0xF824:
      _sensorType=0xF824;
      _sensorName = "THGR810";
      return decode_THGR810(pt); break;
    default:
      std::cout << "Unknown sensor id: " << std::hex << isensorId << std::endl;
      return false;
      break;
    }
  }
  else {
    std::cout << "OSV3 - decode: bad length" << std::endl; 
  }
  return false;
}
// —————————————————————————————
// Decode OregonScientific V3 protocol for specific
// Oregon Devices
// – THGR810 : Temp + Humidity
// ——————————————————————————————
bool OregonSensorV3::decode_THGR810(char * pt) {

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
    printf("OSV3 – decode : id(%s) ch(%c) bat(%c) temp(%s) sign(%c) humid(%s) cksum(%s) crc(%s)\n","A824",channel,battery,temp,tempS,humid, checksum, crc);
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
    printf("OSV3 – decode : id(0x%04X) ch(%d) bat(%d) temp(%f) sign(%d) humid(%f) cksum(0x%02X) crc(0x%02X)\n",0x1D30,ichannel,ibattery,dtemp,itempS,dhumid, ichecksum, icrc);
#endif

    // Check SUM & CRC
    if ( validate(pt,16,icrc,ichecksum) == true ) {

      // now we can decode the important flag and fill the object
      _availableInfos.setFlags(haveChannel);
      _channel = ichannel;
      _availableInfos.setFlags(haveBattery);
  
      if(ibattery & 0x4) {
	_availableInfos.setFlags(battery);
      }
      _availableInfos.setFlags(haveTemperature);
      _temperature = (itempS == 0)?dtemp:-dtemp;
      _availableInfos.setFlags(haveHumidity);
      _humidity = dhumid;
      return true;
    }
    else
      return false;

  }
  return false;
}

// —————————————————–
// Validate CRC and Checksum value from the signal
// Starts at the Sync header digit
// return true if both are valid
// seems to be the same as OSV2 ???
bool OregonSensorV3::validate(char * _str, int _len, int _CRC, int _SUM) {
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
printf("OSV3 – validate err (%s) SUM : 0x%02X(0x%02X) CRC : 0x%02X(0x%02X)\n",_str,SUM,_SUM,CRC,_CRC);
#endif
}
return false;
}
