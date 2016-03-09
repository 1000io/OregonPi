OregonPi
========

433 Sensor Oregon Scientific decoder for Raspberry Pi

Code based on disk91 project http://www.disk91.com

Support OregonSensorV2
----------------------

 - THGR122NX
 - THN132N
 - THGRN228NX
 - WGR9180
 - STR928N
 - BTHG968

Hardware Config
---------------

https://projects.drogon.net/raspberry-pi/wiringpi/pins/

INPUT GPIO 1

Install wiringPi
----------------

 - sudo apt-get install git-core
 - sudo apt-get update
 - sudo apt-get upgrade
 - git clone git://git.drogon.net/wiringPi
 - cd wiringPi
 - git pull origin
 - ./build

Install OregonPi
----------------

 - cd ..
 - git clone git://github.com/1000io/OregonPi
 - cd OregonPi
 - make

Execute
-------

sudo ./test

Execute & Save Log Archive
--------------------------

nohup sudo /home/pi/OregonPi/test /home/pi/OregonPi/log_oregon.csv &

Data Support
------------

Sensor(char * _strval); // construct and decode value

 - bool availableTemp(); // return true if valid && have Temp
 - bool availableHumidity(); // return true if valid && have Humidity
 - bool isBatteryLow(); // return true if valid && haveBattery && flag set.
 - bool hasChannel(); // return true if valid && haveChannel
 - bool isDecoded(); // return true if valide
 - bool availableSpeed(); // return true if valid && speed in km/h
 - bool availableDirection(); // return true if valid && wind direction
 - bool availableRain(); // return true if valid && rain in mm/h
 - bool availablePressure(); // return true if valid && pressure in mb
 - double getTemperature(); // return temperature in CÂ°
 - double getHumidity(); // return humidity in % (base 100)
 - char * getSensorName(); // return sensor name
 - double getRain(); // return Rain
 - double getTrain();
 - double getDirection(); // return wind direction
 - double getSpeed(); // return speed in km/h
 - double getPressure(); // return pressure in mb
 - int getChannel(); // return channel value
 - int getSensClass(); // return sensor class
 - int getSensType(); // return sensor type

Thanks to
---------

@renzo38 & @FunFR


