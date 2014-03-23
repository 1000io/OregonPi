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


