/* ===================================================
 * Sensor.h
 * ====================================================
 * Sensor decoding from 433 Message
 *
 * Created on: 17 sept. 2013
 * Author: disk91 / (c) http://www.disk91.com
 * ===================================================
 */

#ifndef SENSOR_H_
#define SENSOR_H_

#define SENS_CLASS_NONE -1 // Not initialized
#define SENS_CLASS_MTP 0 // MyTeePi virtual or phisical sensor
#define SENS_CLASS_OS 1 // Oregon Scientific

#define SENS_TYP_MTP_CPU 0 // cpu temperature
#define SENS_TYP_MTP_INT 1 // internal temperature

#define SENS_TYP_OS_1D20 0x1D20 // THGR122NX
#define SENS_TYP_OS_EC40 0xEC40 // THN132N
#define SENS_TYP_OS_1D30 0x1D30 // THGRN228NX
#define SENS_TYP_OS_3D00 0x3D00 // WGR9180
#define SENS_TYP_OS_2D10 0x2D10 // STR928N
#define SENS_TYP_OS_5D60 0x5D60 // BTHG968

#include <string>
#include "Flag.h"

class Sensor {

 protected:
    enum {
    battery		= 1 << 0,
    haveTemperature	= 1 << 1,
    haveHumidity	= 1 << 2,
    haveBattery		= 1 << 3,
    haveChannel		= 1 << 4,
    haveDirection	= 1 << 5,
    haveSpeed		= 1 << 6,
    haveRain		= 1 << 7,
    haveTrain		= 1 << 8,
    havePressure	= 1 << 9,
    isValid		= 1 << 10
  } eInfo;

  double _temperature;
  double _humidity;
  double _rain;
  double _train;
  double _direction;
  double _speed;
  double _pressure;

  int _channel;

  int _sensorClass; // marque du sensor cf #define
  int _sensorType; // model of sensor
  std::string _sensorName;

  //store up to 16 bits
  Flags<int16_t> _availableInfos;
  
  // time_t creationTime; // objectCreation time

  virtual bool decode ( char * _str) = 0; // decode the string and set the variable

 protected:
  int getIntFromChar(char c) const ; // transform a Hex value in char into a number
  int getIntFromString(char *) const ; // transform a Hex value in String into a number
  double getDoubleFromString(char *) const ; // transform a BCD string into a double
  

 public:

  Sensor(); // construct and decode value

  virtual ~Sensor() { };
  
  bool availableTemp() const; // return true if valid && have Temp
  bool availableHumidity() const; // return true if valid && have Humidity
  bool isBatteryLow() const; // return true if valid && haveBattery && flag set.
  bool hasChannel() const; // return true if valid && haveChannel
  bool isDecoded() const; // return true if valide
  bool availableSpeed() const; // return true if valid && speed in km/h
  bool availableDirection() const; // return true if valid && wind direction
  bool availableRain() const; // return true if valid && rain in mm/h
  bool availablePressure() const; // return true if valid && pressure in mb

  double getTemperature() const; // return temperature in CÂ°
  double getHumidity() const; // return humidity in % (base 100)
  const std::string& getSensorName() const; // return sensor name
  double getRain() const; // return Rain
  double getTrain() const;
  double getDirection() const; // return wind direction
  double getSpeed() const; // return speed in km/h
  double getPressure() const; // return pressure in mb

  int getChannel() const; // return channel value
  int getSensClass() const; // return sensor class
  int getSensType() const; // return sensor type

  //time_t getCreationTime(); // return object creation time

  static Sensor * getRightSensor(char * s); // wrapper for child class

};

class OregonSensorV2 : public Sensor {
 public :
  OregonSensorV2(char * _strval);
  static const char _sensorId[];
					  
 private:
  bool decode( char * _str ); // wrapper to right decode method

  bool decode_BTHG968(char *pt); // decode sensor information
  bool decode_RGR918(char *pt); // decode sensor information
  bool decode_THGR122NX(char * pt); // decode sensor informations
  bool decode_THN132N(char * pt); // decode sensor informations
  bool decode_THGRN228NX(char * pt); // decode sensor informations
  bool decode_WGR918(char * pt); // decode sensor informations
  bool validate(char * _str, int _len, int _CRC, int _SUM); // Verify CRC & CKSUM
};

class OregonSensorV3 : public Sensor {
 public :
  OregonSensorV3(char * _strval);
  static const char _sensorId[];

 private:
  bool decode( char * _str ); // wrapper to right decode method
  bool validate(char * _str, int _len, int _CRC, int _SUM); // Verify CRC & CKSUM

  bool decode_THGR810(char* pt); // decode sensor informations
};

#endif /* SENSOR_H_ */
