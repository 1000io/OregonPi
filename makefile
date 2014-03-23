all: test

test: RCSwitch.o RcOok.o Sensor.o test.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $+ -o $@ -lwiringPi

clean:
	$(RM) *.o test
