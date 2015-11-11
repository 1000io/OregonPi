CXX=g++-4.8
LD=g++-4.8

NANODBC_HOME=../nanodbc/src

CXXFLAGS += -O2 -Wall -g -pthread -I $(NANODBC_HOME) -std=c++11

all: getSensorData

getSensorData: RCSwitch.o RcOok.o Sensor.o DataServer.o sensorData.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $+ -o $@ -lwiringPi

clean:
	$(RM) *.o test
