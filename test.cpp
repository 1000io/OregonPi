/* ===================================================
C code : test.cpp
* ===================================================
*/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include "RCSwitch.h"
#include "RcOok.h"
#include "Sensor.h"

int main(int argc, char *argv[])
{
	int RXPIN = 1;
	int TXPIN = 0;
        int loggingok;   // Global var indicating logging on or off
        FILE *fp;        // Global var file handle
        
        if(argc==2) {
          fp = fopen(argv[1], "a"); // Log file opened in append mode to avoid destroying data
          loggingok=1;
          if (fp == NULL) {
              perror("Failed to open log file!"); // Exit if file open fails
              exit(EXIT_FAILURE);
          }
        } else {
          loggingok=0;
        }

	if(wiringPiSetup() == -1)
		return 0;

	RCSwitch *rc = new RCSwitch(RXPIN,TXPIN);

	while (1)
	{
		if (rc->OokAvailable())
		{
			char message[100];

			rc->getOokCode(message);
			printf("%s\n",message);

			Sensor *s = Sensor::getRightSensor(message);
			if (s!= NULL)
			{
				printf("Temp : %f\n",s->getTemperature());
				printf("Humidity : %f\n",s->getHumidity());
				printf("Channel : %d\n",s->getChannel());
                                if((loggingok) && (s->getChannel()>0)) {
                                        fprintf(fp,"%d,temp%f,hum%f\n",s->getChannel(),s->getTemperature(),s->getHumidity());
                                        fflush(fp);
                                        fflush(stdout);
                                }
			}
			delete s;
		}
		delay(1000);
	}
}
